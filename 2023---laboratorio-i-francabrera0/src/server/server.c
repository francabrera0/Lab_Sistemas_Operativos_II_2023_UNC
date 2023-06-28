#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>
#include <bits/sigaction.h>

#include "../../include/utilities/readConfig.h"
#include "../../include/utilities/msgStructs.h"
#include "../../include/utilities/semStructs.h"
#include "../../include/utilities/shmStructs.h"
#include "../../include/utilities/colors.h"

#define CHANNELS 4

int ctrl = 1;
char* paths[CHANNELS] = {NULL, NULL, NULL, NULL};
char* keys[CHANNELS] = {"requestQueue", "msgQueue", "shMemory", "fifo"};
enum {REQUEST, QUEUE, MEMORY, FIFO};

static int createFiles();
static int initializeQueue(MessageQueue* requestQueue, Semaphore* semaphoreRequestQueue, char* path);
static int initializeShMemory(ShMemory* requestQueue, Semaphore* semaphoreRequestQueue, char* path);
static int initializeFifo(Semaphore* semaphore, char* path);
static int initializeSemaphore(Semaphore* semaphore, key_t token);
static int checkRequestQueue(MessageQueue* requestQueue);
static int checkMessageQueue(MessageQueue* queue);
static int checkSharedMemory(ShMemory* memory);
static int checkFifo(int fd);
static void printCounter(int counter[CHANNELS]);
static char* responseMessage(long type);
void sigIntHandler(int signum);


int main() {

    int counter[CHANNELS] = {0, 0, 0, 0};

    if(createFiles(paths))
        exit(EXIT_FAILURE);

    MessageQueue requestQueue;
    Semaphore semaphoreRequestQueue;
    if((initializeQueue(&requestQueue, &semaphoreRequestQueue, paths[REQUEST])) == 1)
        exit(EXIT_FAILURE);

    MessageQueue queue;
    Semaphore semaphoreQueue;
    if((initializeQueue(&queue, &semaphoreQueue, paths[QUEUE])) == 1)
        exit(EXIT_FAILURE);

    ShMemory memory;
    Semaphore semaphoreMemory;
    if((initializeShMemory(&memory, &semaphoreMemory, paths[MEMORY])) == 1)
        exit(EXIT_FAILURE);
    
    Semaphore semaphoreFifo;
    int fd = initializeFifo(&semaphoreFifo, paths[FIFO]);
    if(fd == -1)
        exit(EXIT_FAILURE);

    struct sigaction sa;
    sa.sa_handler = sigIntHandler;
    sigaction(SIGINT, &sa, NULL);

    while(ctrl) {
        if(!(checkRequestQueue(&requestQueue)))
            counter[0]++;
        if(!(checkMessageQueue(&queue)))
            counter[1]++;
        if(!(checkSharedMemory(&memory)))
            counter[2]++;
        if(!(checkFifo(fd)))
            counter[3]++;
    }

    shmdt(memory.shmAddress);
    msgctl(requestQueue.id, IPC_RMID, NULL);
    msgctl(queue.id, IPC_RMID, NULL);
    shmctl(memory.id, IPC_RMID, NULL);
    semctl(semaphoreRequestQueue.id, 0, IPC_RMID, NULL);
    semctl(semaphoreQueue.id, 0, IPC_RMID, NULL);
    semctl(semaphoreMemory.id, 0, IPC_RMID, NULL);
    semctl(semaphoreFifo.id, 0, IPC_RMID, NULL);
    close(fd);
    
    printCounter(counter);
    
    printf("Press return to exit");
    getchar();

    return 0;
}

/**
 * @brief Obtain all necessary paths to initialize communication channels and create files with these names
 * 
 * @param none
 * @return On success, 0 is returned. On failure, 1 is returned
*/
static int createFiles() {
    for(int i=0; i<CHANNELS; i++) {
        paths[i] = getConfig(keys[i]);
        if(paths[i] == NULL) {
            perror("Could not get path");
            return 1;
        }
        if(i==3)
            continue;
        FILE* file;
        if((file = fopen(paths[i], "w")) == NULL) {
            perror("Could not create file");
            return 1;
        }
        fclose(file);
    }
    return 0;
}


/**
 * @brief Read the requestQueue, in case there is a request respond with the appropiate path
 * 
 * @param requestQueue MessageQueue struct pointer 
 * @return int On success 0 is returned. On failure 1 is returned
 */
static int checkRequestQueue(MessageQueue* requestQueue) {
    if(msgrcv(requestQueue->id, &requestQueue->msg, sizeof(requestQueue->msg.text), 0, IPC_NOWAIT) != -1) {
        printf("Request received queue %d: %s, type %ld\n", requestQueue->id, requestQueue->msg.text, requestQueue->msg.type);
            
        long type = requestQueue->msg.type;
        requestQueue->msg.type = 1;

        strcpy(requestQueue->msg.text, responseMessage(type));
            
        if(msgsnd(requestQueue->id, &requestQueue->msg, sizeof(requestQueue->msg.text), 0) == -1) {
            perror("Invalid request response message");
            return 1;
        }
        return 0;
    }
    return 1;
}

/**
 * @brief Read the messageQueue, if there is a message, it prints it
 * 
 * @param queue MessageQueue struct pointer
 * @return int On success, 0 is returned. On failure 1 is returned
 */
static int checkMessageQueue(MessageQueue* queue) {
    if(msgrcv(queue->id, &  queue->msg, sizeof( queue->msg.text), 0, IPC_NOWAIT) != -1) {
        printf(BLUE"Message received from msgQueue %d: %s\n"RESET_COLOR, queue->id, queue->msg.text);
        return 0;
    }
    return 1;
}

/**
 * @brief Read the sharedMemory segment, if there is a message, it prints it
 * 
 * @param memory ShMemory struct pointer
 * @return int On success, 0 is returned. On failure 1 is returned
 */
static int checkSharedMemory(ShMemory* memory) {
    char buffer[60];
    strcpy(buffer, (char*) memory->shmAddress);
    if(strcmp(buffer, "*") && strcmp(buffer, "")){
        printf(GREEN"Message received from shared memory %d: %s\n"RESET_COLOR, memory->id, buffer);
        strcpy((char*) memory->shmAddress, "*");
        return 0;
    }
    return 1;
}

/**
 * @brief Read the fifo, if there is a message, it prints it
 * 
 * @param fd file descriptor of fifo
 * @return int On succes 0 is returned. On failure 1 is returned.
 */
static int checkFifo(int fd) {
    char buffer[20];
    ssize_t n = read(fd, buffer, sizeof(buffer));
    if(n>0) {
        printf(RED"Message received from fifo %d: %s\n"RESET_COLOR, fd, buffer); 
        return 0;
    }
    return 1;
}


/**
 * @brief Initialize the queue message communication channel
 * 
 * @param queue MessageQueue struct pointer
 * @param semaphore Semaphore struct pointer
 * @param path path to file for ftok function
 * @return int On success, 0 is returned. On failure 1 is returned
 */
static int initializeQueue(MessageQueue* queue, Semaphore* semaphore, char* path) {
    key_t token = ftok(path, 'A');
    if(token == -1) {
        perror("Culd not create a token for queue");
        return 1;
    }
    queue->key = token;
    if((queue->id = msgget(queue->key, 0666 | IPC_CREAT)) == -1) {
        perror("Could not create an id for queue");
        return 1;
    }
    if(initializeSemaphore(semaphore, token))
        return 1;

    return 0;
}

/**
 * @brief Initialize the Sahred Memory communication channel
 * 
 * @param memory ShMemory struct pointer
 * @param semaphore Semaphore struct pointer
 * @param path Path to file for ftok function
 * @return int On success, 0 is returned. On failure 1 is returned
 */
static int initializeShMemory(ShMemory* memory, Semaphore* semaphore, char* path) {
    key_t token = ftok(path, 'A');
    if(token == -1) {
        perror("Culd not create a token for shMemory");
        return 1;
    }
    memory->key = token;
    if((memory->id = shmget(memory->key,60 ,0666 | IPC_CREAT)) == -1) {
        perror("Could not create an id for shMemory");
        return 1;
    }
    if((memory->shmAddress = shmat(memory->id, NULL, 0)) == (char*) -1) {
        perror("Could not attach shared memory");
        return 1;
    }
    if(initializeSemaphore(semaphore, token))
        return 1;

    return 0;
}

/**
 * @brief Initialize the fifo channel communication
 * 
 * @param semaphore Semaphore struct pointer
 * @param path Path to file for ftok function
 * @return int On succes file descriptor is returned. On failure -1 is returned.
 */
static int initializeFifo(Semaphore* semaphore, char* path) {
    if((mkfifo(path, 0666)) == -1){
        if(errno != EEXIST) {
            perror("Could not create fifo");
            return -1;
        }
    }
    int fd = open(path, O_RDONLY | O_NONBLOCK);
    if(fd ==-1) {
        perror("Could not open fifo");
        return -1;
    }

    key_t token = ftok(path, 'A');
    if(token == -1) {
        perror("Culd not create a token for fifo");
        return -1;
    }
    if(initializeSemaphore(semaphore, token))
        return -1;


    return fd;
}

/**
 * @brief Initialize the semaphore passed as an argument and take one token
 * 
 * @param semaphore Semaphore struct pointer
 * @param token token obtain by ftok function
 * @return int On success, 0 is returned. On failure 1 is returned
 */
static int initializeSemaphore(Semaphore* semaphore, key_t token) {
    semaphore->key = token;
    if((semaphore->id = semget(semaphore->key, 1, 0666 | IPC_CREAT)) == -1) {
        perror("Could not create an id for semaphore");
        return 1;
    }
    if((semctl(semaphore->id, 0, SETVAL, 2)) == -1) {
        perror("Could not initialize a semaphore");
        return 1;
    }
    struct sembuf sb = {0, -1, 0};
    if((semop(semaphore->id, &sb, 1)) == -1) {
        perror("Could not acquire semaphore");
        return 1;
    }
    return 0;
}

/**
 * @brief return a specific message based on the reqested channel
 * 
 * @param type type of channel
 * @return char* path to file to generate the corresponding token
 */
static char* responseMessage(long type) {
    switch (type) {
    case 1:
        return(paths[QUEUE]);
        break;
    case 2:
        return(paths[MEMORY]);
        break;
    case 3:
        return(paths[FIFO]);
        break;
    default:
        return("Error");
        break;
    }
}

/**
 * @brief Print the number of messages received by each channel
 * 
 * @param counter int array with the number of messages
*/
static void printCounter(int counter[CHANNELS]) {
    printf("----------Server Report--------\n");
    printf("Messages in requestQueue: %d\n", counter[REQUEST]);
    printf("Messages in messageQueue: %d\n", counter[QUEUE]);
    printf("Messages in sharedMemory: %d\n", counter[MEMORY]);
    printf("Messages in fifo: %d\n", counter[FIFO]);
    printf("-------------------------------\n");
}

/**
 * @brief Signal handler for SIGINT
 * 
 * @param signum 
 */
void sigIntHandler(int signum) {
    printf("Server shutdown %d\n", signum);
    ctrl = 0;
}