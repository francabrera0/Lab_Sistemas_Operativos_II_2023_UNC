#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <signal.h>
#include <bits/sigaction.h>
#include "../../include/client/client.h"

#define CHANNEL 1
int ctrl = 1;

static int initializeChannel(MessageQueue* queue, char* pathFile);
static int sendMessage(MessageQueue queue ,const char* procName);
void sigIntHandler(int signum);

int main(int argc, char const *argv[]) {
        
    if(argc<2) {
        perror("Invalid arguments\n");
        exit(EXIT_FAILURE);
    }
    const char* procName = argv[1];

    MessageQueue requestQueue;
    Semaphore semaphoreRequestQueue;
    char* response = sendRequest(requestQueue, semaphoreRequestQueue, CHANNEL, procName);
    if(!strcmp(response, "Error") || !strcmp(response, "NULL"))
        exit(EXIT_FAILURE);

    MessageQueue queue;
    Semaphore semaphore;
    if(initializeChannel(&queue, response))
        exit(EXIT_FAILURE);
    if(initializeSemaphore(&semaphore, response))
        exit(EXIT_FAILURE);
    
    struct sigaction sa;
    sa.sa_handler = sigIntHandler;
    sigaction(SIGINT, &sa, NULL);

    while (ctrl) {
        if(semaphoreAcquire(semaphore))
            exit(EXIT_FAILURE);
        int fail = sendMessage(queue, procName);
        if(semaphoreRelease(semaphore))
            exit(EXIT_FAILURE);
        sleep(2);
        if(fail)
            exit(EXIT_FAILURE);
    }
    
    sleep(2);   

    return 0;
}

/**
 * @brief Initialize the channel communication
 * 
 * @param queue MessageQueue struct pointer
 * @param pathFile Path to file to generate token through ftok function
 * @return int On success 0 is returned. On failure 1 is returned
 */
static int initializeChannel(MessageQueue *queue, char* pathFile) {
    if((queue->key = ftok(pathFile, 'A')) == -1) {
        perror("Could not create a token for messageQueue");
        return 1;
    }

    if((queue->id = msgget(queue->key, 0666 | IPC_CREAT)) == -1) {
        perror("Could not create an id for messageQueue");
        return 1;
    }
    queue->msg.type = 1;
    return 0;
}

/**
 * @brief Send a message through the channel communication to the server
 * 
 * @param queue MessageQueue struct
 * @param procName Name of the process that sends the message
 * @return int On success 0 is returned. On failure 1 is returned
 */
static int sendMessage(MessageQueue queue, const char* procName) {
    static int i = 0;
    char msgToSend[60];
    sprintf(msgToSend, "% d ", i);
    i++;
    strcat(msgToSend, procName);
    strcpy(queue.msg.text, msgToSend);
    if(msgsnd(queue.id, &queue.msg, sizeof(queue.msg.text), IPC_NOWAIT) == -1) {
        return 1;
    }
    return 0;
}

/**
 * @brief Signal handler for SIGINT
 * 
 * @param signum 
 */
void sigIntHandler(int signum) {
    printf("Client shutdown %d\n", signum);
    ctrl = 0;
}