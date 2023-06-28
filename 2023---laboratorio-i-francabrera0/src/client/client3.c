#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <signal.h>
#include <bits/sigaction.h>

#include "../../include/client/client.h"

#define CHANNEL 3
int ctrl = 1;

static int initializeChannel(char* path);
static int sendMessage(int fd, const char* procName);
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
    
    int fd;
    Semaphore semaphore;
    if((fd = initializeChannel(response)) == -1) 
        exit(EXIT_FAILURE);
    if((initializeSemaphore(&semaphore, response)) == 1)
        exit(EXIT_FAILURE);
    
    struct sigaction sa;
    sa.sa_handler = sigIntHandler;
    sigaction(SIGINT, &sa, NULL);

    while (ctrl) {
        if(semaphoreAcquire(semaphore))
            exit(EXIT_FAILURE);
        sendMessage(fd, procName);
        if(semaphoreRelease(semaphore))
            exit(EXIT_FAILURE);
        sleep(2);

    }
    
    sleep(2);   

    return 0;
}

/**
 * @brief Initialize the channel communication
 * 
 * @param path Path to file to create fifo
 * @return int On success file descriptor is returned. On failure -1 is returned
 */
static int initializeChannel(char* path) {
    if((mkfifo(path, 0666)) == -1){
        if(errno != EEXIST) {
            perror("Could not create fifo");
            return -1;
        }
    }
    int fd = open(path, O_WRONLY);
    if(fd ==-1) {
        perror("Could not open fifo");
        return -1;
    }

    return fd;
}

/**
 * @brief Send a message through the channel communication to the server
 * 
 * @param fd Fifo file descriptor 
 * @param procName Name of the process that sends the message
 * @return int On success 0 is returned. On failure 1 is returned
 */
static int sendMessage(int fd, const char* procName) {
    static int i = 0;
    char msgToSend[20];
    sprintf(msgToSend, "% d ", i);
    i++;
    strcat(msgToSend, procName);
    if((write(fd, msgToSend, sizeof(msgToSend))) == -1)
        return 1;
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