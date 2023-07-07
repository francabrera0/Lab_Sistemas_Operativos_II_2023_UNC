#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

static int curl(char *method, char *url, char *data);

/**
 * @brief 
 * 
 * @param argc argument counter
 * @param argv client name, server host and port number
 * @return int On success EXIT_SUCCESS is returned. On failure EXIT_FAILURE is returned
 */
int main(int argc, const char *argv[]) {
    if(argc != 4) {
        perror("Please enter the client name, server host and server port number");
        exit(EXIT_FAILURE);
    }

    const char *clientName = argv[1];
    const char *host = argv[2];
    int port = atoi(argv[3]);
    
    char url[64];
    sprintf(url, "http://%s:%d/", host, port);
    char data[64];
    sprintf(data, "name=%s", clientName);

    while (1) {
        char opt = (char)getchar();
        int c;
        while((c = getchar()) != '\n' && c != EOF);
        switch (opt) {
        case 'g':
            if(curl("print", url, data)) {
                exit(EXIT_FAILURE);
            }
            break;
        case 'p':
            if(curl("increment", url, data)) {
                exit(EXIT_FAILURE);
            }
            break;
        case 'e':
            exit(EXIT_SUCCESS);
            break;
        default:
            printf("Invalid option. Enter 'g' for get method, 'p' for post method or 'e' for exit\n");
            break;
        }
    }
}

/**
 * @brief Perform a curl recuest
 * 
 * @param method endpoint, this parameter must be "increment" or "print"
 * @param url server url
 * @param data post parameters
 * @return int On success 0 is returned. On failure 1 is returned
 */
static int curl(char *method, char *url, char *data) {
    CURL *curl = curl_easy_init();
    char *urltmp = (char*)malloc(sizeof(char)*32);
    strcpy(urltmp, url);
    if(curl) {
        if(!strcmp(method, "increment")) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
            strcat(urltmp, method);
        }
        else {
            strcat(urltmp, method);
        }
        curl_easy_setopt(curl, CURLOPT_URL, urltmp);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, stdout);
        free(urltmp);
        CURLcode response = curl_easy_perform(curl);
        if(response != CURLE_OK) {
            perror("Could not perform curl");
            return 1;
        }
        curl_easy_cleanup(curl);
        return 0;
    }
    perror("Could not init curl");
    return 1;
}