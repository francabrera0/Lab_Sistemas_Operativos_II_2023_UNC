#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/sem.h>

#include "../../include/client/client.h"
#include "../../include/utilities/readConfig.h"
#include "../../include/utilities/msgStructs.h"
#include "../../include/utilities/semStructs.h"

/**
 * @brief Send a request to the Server through a messageQueue, then waits for its response
 * 
 * @param requestQueue MessageQueue struct
 * @param semaphoreRequestQueue Semaphore struct (to synchronize queue access)
 * @param type Type of channel communication
 * @param procName Name of the process that make the request
 * @return char* On success the response from server is returned. On failure "NULL" is returned
 */
char* sendRequest(MessageQueue requestQueue, Semaphore semaphoreRequestQueue, long type, const char* procName) {

    char* requestPath = getConfig("requestQueue");
    if(requestPath == NULL) {
        perror("Could not obtain path to requestQueue");
        exit(EXIT_FAILURE);
    }

    if((requestQueue.key = ftok(requestPath, 'A')) == -1) {
        perror("Could not create a token for requestQueue");
        return "NULL";
    }

    if((requestQueue.id = msgget(requestQueue.key, 0666 | IPC_CREAT)) == -1) {
        perror("Could not create an id for requestQueue");
        return "NULL";
    }
    
    requestQueue.msg.type = type;
    char requestSnd[60];
    sprintf(requestSnd,"I ask for communication %s", procName);
    strcpy(requestQueue.msg.text, requestSnd);

    if((initializeSemaphore(&semaphoreRequestQueue, requestPath)))
        return "NULL";

    if((semaphoreAcquire(semaphoreRequestQueue)))
        return "NULL";
    sleep(1);
    if(msgsnd(requestQueue.id, &requestQueue.msg, sizeof(requestQueue.msg.text), 0) == -1) {
        perror("Invalid request message");
        semaphoreRelease(semaphoreRequestQueue);
        return "NULL";
    }

    if (msgrcv(requestQueue.id, &requestQueue.msg, sizeof(requestQueue.msg.text), 1, 0) == -1){
        perror("Invalid request response message");
        semaphoreRelease(semaphoreRequestQueue);
        return "NULL";
    }

    if((semaphoreRelease(semaphoreRequestQueue)))
        return "NULL";

    printf("Server request response %s\n", requestQueue.msg.text);
    char* request = (char*)malloc(60*sizeof(char));
    strcpy(request, requestQueue.msg.text);
    return request;
}

/**
 * @brief Initialize the semaphore passed as an argument
 * 
 * @param semaphore Semaphore struct pointer
 * @param path Path to file to generate token through ftok function
 * @return int On success 0 is returned. On failure 1 is returned
 */
int initializeSemaphore(Semaphore* semaphore, char* path) {
 
    if((semaphore->key = ftok(path, 'A')) == -1) {
        perror("Could not create a token for semaphore");
        return 1;
    }
    if((semaphore->id = semget(semaphore->key, 1, 0666 | IPC_CREAT)) == -1) {
        perror("Could not create an id for semaphore");
        return 1;
    }
    return 0;
}

/**
 * @brief Realize the acquire of the semaphore passed as an argument
 * 
 * @param semaphore Semaphore struct
 * @return int On success 0 is returned. On failure 1 is returned
 */
int semaphoreAcquire(Semaphore semaphore) {
    struct sembuf sb = {0, -1, 0};
    if((semop(semaphore.id, &sb, 1)) == -1) {
        perror("Could not acquire semaphore");
        return 1;
    }
    return 0;
}

/**
 * @brief Realize the release of the semaphore passed as an argument
 * 
 * @param semaphore Semaphore struct
 * @return int On success 0 is returned. On failure 1 is returned
 */
int semaphoreRelease(Semaphore semaphore) {
    struct sembuf sb = {0, 1, 0};
    if((semop(semaphore.id, &sb, 1)) == -1) {
        perror("Could not release semaphore");
        return 1;        
    }
    return 0;
}