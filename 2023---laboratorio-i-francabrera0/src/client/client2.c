#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <signal.h>
#include <bits/sigaction.h>

#include "../../include/client/client.h"
#include "../../include/utilities/shmStructs.h"

#define CHANNEL 2
int ctrl = 1;

static int initializeChannel(ShMemory* queue, char* pathFile);
static int sendMessage(ShMemory memory ,const char* procName);
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

    ShMemory memory;
    Semaphore semaphore;
    if(initializeChannel(&memory, response))
        exit(EXIT_FAILURE);
    if(initializeSemaphore(&semaphore, response))
        exit(EXIT_FAILURE);
    
    struct sigaction sa;
    sa.sa_handler = sigIntHandler;
    sigaction(SIGINT, &sa, NULL);

    while (ctrl) {
        if(semaphoreAcquire(semaphore))
            exit(EXIT_FAILURE);
        sendMessage(memory, procName);
        if(semaphoreRelease(semaphore))
            exit(EXIT_FAILURE);
        sleep(2);
    }
    
    shmdt(memory.shmAddress);
    sleep(2);   

    return 0;
}

/**
 * @brief Initialize the channel communication
 * 
 * @param memory ShMemory struct pointer
 * @param pathFile Path to file to generate token through ftok function
 * @return int On success 0 is returned. On failure 1 is returned
 */
static int initializeChannel(ShMemory *memory, char* pathFile) {
    if((memory->key = ftok(pathFile, 'A')) == -1) {
        perror("Could not create a token for shMemory");
        return 1;
    }

    if((memory->id = shmget(memory->key,60 ,0666 | IPC_CREAT)) == -1) {
        perror("Could not create an id for shMemory");
        return 1;
    }

    if((memory->shmAddress = shmat(memory->id, NULL, 0)) == (char*) -1) {
        perror("Could not attach shared memory");
        return 1;
    }
    return 0;
}

/**
 * @brief Send a message through the channel communication to the server
 * 
 * @param memory ShMemory struct 
 * @param procName Name of the process that sends the message
 * @return int On success 0 is returned. On failure 1 is returned
 */
static int sendMessage(ShMemory memory, const char* procName) {
    static int i = 0;
    char msgToSend[60];
    sprintf(msgToSend, "% d ", i);
    i++;
    strcat(msgToSend, procName);
    strcpy((char*) memory.shmAddress, msgToSend);
    while(*(char*) memory.shmAddress != '*')
        sleep(1);
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