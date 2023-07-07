#include <stdio.h>
#include <ulfius.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <time.h>
#include <semaphore.h>
#define PORT 8537

/*Global variables*/
int counter = 0;
char lastUpdate[32];
struct tm *actualTime;
FILE *logFile;
sem_t semaphore;

/*Endpoint functions definition*/
int callback_increment (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_print (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_default (const struct _u_request * request, struct _u_response * response, void * user_data);

/**
 * @brief Main function, initialice server
 * 
 * @param argc Argument counter
 * @param argv Argument value, argv[1] must be the port number
 * @return int On success EXIT_SUCCESS is returned. On failure EXIT_FAILURE is returned
 */
int main(int argc, const char *argv[]) {

    if(argc != 2) {
        perror("Please, enter a port number");
        exit(EXIT_FAILURE);
    }
    
    logFile = fopen("log", "w");
    if(logFile == NULL) {
        perror("Could not open log file");
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    if(!port) {
        perror("Invalid argument");
        exit(EXIT_FAILURE);
    }

    struct _u_instance instance;

    sem_init(&semaphore, 0, 1);

    if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
        perror("Could not create instance");
        exit(EXIT_FAILURE);
    }

    ulfius_add_endpoint_by_val(&instance, "GET", "/print", NULL, 0, &callback_print, NULL);
    ulfius_add_endpoint_by_val(&instance, "POST", "/increment", NULL, 0, &callback_increment, NULL);
    ulfius_set_default_endpoint(&instance, &callback_default, NULL);

    time_t now = time(NULL);
    actualTime = localtime(&now);

    if (ulfius_start_framework(&instance) == U_OK) {
        printf("Start server on port %d\n", instance.port);
        getchar();
    } 
    else {
        perror("Error starting server\n");
        exit(EXIT_FAILURE);
    }
    printf("End server\n");

    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);

    return(EXIT_SUCCESS);
}

/**
 * @brief Validate post parameters like key=value
 * 
 * @param key key
 * @param value value
 * @return int On success 0 is returned. On failure 1 is returned
 */
static int validateParams(char* key, char* value) {
    if(key == NULL || value == NULL) {
        return 1;
    }
    return 0;
}

/**
 * @brief Create a string in json format to send to a client
 * 
 * @param counter Global counter
 * @param update Name of the last client that updated the counter
 * @param method Http method, this could be "GET", or "POST"
 * @param actualTime Time of the last update
 * @param jsonResponse Char* to save json string
 */
static void jsonFormat(int counter, char* update, char* method, struct tm *actualTime, char *jsonResponse) {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "Method", method);
    cJSON_AddNumberToObject(root, "Counter value", counter);
    cJSON_AddStringToObject(root, "Last update", update);
    char time[80];
    strftime(time, 80, "%d-%m-%Y %H:%M:%S", actualTime);
    cJSON_AddStringToObject(root, "Time", time);
    char* print = cJSON_Print(root);
    strcpy(jsonResponse, print);
    strcat(jsonResponse, "\n");
    fprintf(logFile, "%s",jsonResponse);
    free(print);
    cJSON_Delete(root);
}

/**
 * @brief Increment the counter value and update client name and time of this update
 * 
 * @param key key
 * @param value client name
 * @param response _u_response struct to send a response to client
 * @return int On success 0 is returned. On failure 1 is returned
 */
static int incrementCounter(char* key, char* value, struct _u_response *response) {
    if(validateParams(key, value)) {
        char msg[64];
        sprintf(msg, "Increment error please send parameters \"name=clientName\" \n");
        ulfius_set_string_body_response(response, 500, msg);
        return 1;
    }
    /*Sync*/
    sem_wait(&semaphore);
    counter++;
    strcpy(lastUpdate, value);
    time_t now = time(NULL);
    actualTime = localtime(&now);
    char *jsonResponse = (char*)malloc(sizeof(char)*128);
    jsonFormat(counter, lastUpdate, "POST", actualTime, jsonResponse);
    ulfius_set_string_body_response(response, 200, jsonResponse);
    sem_post(&semaphore);
    /*End sync*/
    free(jsonResponse);
    return 0;
}

/**
 * @brief Increments the counter value and update the information related with this
 * 
 * @param request _u_request struct
 * @param response _u_response struct
 * @param user_data user_data
 * @return int On success U_CALL_BACK_CONTINUE is returned. On failure U_CALL_BACK_ERROR is returned
 */
int callback_increment (const struct _u_request * request, struct _u_response * response, void * user_data) {
    (void)user_data;
    struct _u_map postParam = *request->map_post_body;
    char* key = postParam.keys[0];
    char* value = postParam.values[0];
    if(incrementCounter(key, value, response)) {
        return U_CALLBACK_ERROR;
    }
    return U_CALLBACK_CONTINUE;
}

/**
 * @brief Print endpoint, return the counter value, and data about the last update
 * 
 * @param request _u_request struct
 * @param response _u_response struct
 * @param user_data user_data
 * @return int U_CALL_BACK_CONTINUE
 */
int callback_print (const struct _u_request * request, struct _u_response * response, void * user_data) {
    (void)request;
    (void)user_data;
    char *jsonResponse = (char*)malloc(sizeof(char)*128);
    jsonFormat(counter, lastUpdate, "GET", actualTime, jsonResponse);
    ulfius_set_string_body_response(response, 200, jsonResponse);
    free(jsonResponse);
    return U_CALLBACK_CONTINUE;
}

/**
 * @brief Default function
 * 
 * @param request _u_request struct
 * @param response _u_response struct
 * @param user_data user_data
 * @return int U_CALL_BACK_CONTINUE
 */
int callback_default (const struct _u_request * request, struct _u_response * response, void * user_data) {
    (void)request;
    (void)user_data;
    ulfius_set_string_body_response(response, 404, "Page not found");
    return U_CALLBACK_CONTINUE;
}
