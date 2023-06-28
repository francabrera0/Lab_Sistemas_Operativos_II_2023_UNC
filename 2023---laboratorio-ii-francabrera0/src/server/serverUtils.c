#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <cjson/cJSON.h>
#include "serverUtils.h"

static char* execJournalctl(char* args, size_t* size);
static void getArguments(char* jsonString, char* args, int isCompressed);
static void removeQuotes(char* str);
static char* getResponse();
static void getCheckSum(char* msg, size_t size, char* checksum);

/**
 * @brief Receive a message from client. Received arguments are decoded and stored in the char** passed as
 *          a parameter
 * 
 * @param socketFd Socket file descriptor
 * @param args Char** to save arguments received
 * @return int On succes, 0 is returned. On failuer, -1 is returned
 */
int receiveMessage(int socketFd, char* args, CLIENTS client, int isCompressed) {
    char buffer[BUFFSIZE];
    memset(buffer, '\0', BUFFSIZE);
    ssize_t size = read(socketFd, buffer, sizeof(buffer));
                
    if (size < 0) {
        perror(RESET_COLOR"Could not read from socket");
        close(socketFd);
        return -1;
    }

	//Test checksum - I don't like how it is, but it works
    size_t separator = strlen(buffer)-32;
    char message[separator+1];
    strncpy(message, buffer, separator);
    message[separator] = '\0';
    char checksum[33];
    strncpy(checksum, buffer+separator, 33);

    char check[33];
    getCheckSum(message, strlen(message), check);
    if(strcmp(checksum, check)){
        perror("Checksum does not match");
        return -1;
    }
    //

    switch (client) {
    case UNIX:
        printf(GREEN"Process %d. Received: %s\n", getpid(), message);
        break;
    case IPV4:
        printf(BLUE"Process %d. Received: %s\n", getpid(), message);
        break;
    case IPV6:
        printf(YELLOW"Process %d. Received: %s\n", getpid(), message);
        break;
    default:
        break;
    }

    if(client != UNIX)
        getArguments(message, args, isCompressed);

    if(!strcmp("end", message)) {
        printf(RED"Process %d. Received end, execution finished of client %d.\n\n", getpid(), client);
        close(socketFd);
        return -1;
    }
    return 0;
}

/**
 * @brief Send a message to the client. The content of this message is the output of running journalctl.
 * 
 * @param socketFd Socket file descriptor
 * @param args Arguments to run journalctl
 * @return int On success, 0 is returned. On failure, -1 is returned.
 */
int sendMessage(int socketFd, char* args, CLIENTS client) {
    char* response = NULL;
    size_t bufferSize = 0;
    if(client==UNIX) {
        response = getResponse();
        bufferSize = strlen(response);
    }
    else
        response = execJournalctl(args, &bufferSize);
        
    if(response == NULL) {
        close(socketFd);
        return -1;
    }
    
    char checksum[33];
    getCheckSum(response, bufferSize, checksum);
    size_t newBufferSize = bufferSize + 33;
    char* newResponse = realloc(response, newBufferSize);
    response = newResponse;
    for(int i=0; i<33; i++) 
        *(response+bufferSize+i) = *(checksum+i); 

    ssize_t size = write(socketFd, response, newBufferSize);
    if (size < 0) {
        perror(RESET_COLOR"Could not write in socket");
        close(socketFd);
        free(response);
        return -1;
    }
    free(response);
    return 0;
}

/**
 * @brief Execute journalctl with the args passed as a parameter. 
 *          The output of this run is saved in char* passed as a parameter.
 * 
 * @param args Arguments to run journalctl
 * @param response Char* to save the output value
 */
static char* execJournalctl(char* args, size_t* size) {
    
    FILE* execution;
    execution = popen(args, "r");
    ssize_t bytesRead;
    size_t bufferSize = BUFFSIZE;
    *size = 0;        
    char* buffer = malloc(bufferSize+1);
    memset(buffer, '\0', bufferSize);
    if(!buffer){
        perror(RESET_COLOR"Could not allocate memory");
        return NULL;
    }
    
    while ((bytesRead = read(fileno(execution), buffer + *size, BUFFSIZE)) > 0) {
        *size += (size_t)bytesRead;
        if(*size >= bufferSize) {
            size_t newBufferSize = bufferSize + BUFFSIZE;
            char* newBuffer = realloc(buffer, newBufferSize);
            if(!newBuffer) {
                perror(RESET_COLOR"Could not reallocate memory");
                free(buffer);
                return NULL;
            }
            buffer = newBuffer;
            bufferSize = newBufferSize;
            memset(buffer+*size, '\0', BUFFSIZE);
        }
    }

    pclose(execution);
    if(*size == 0) {
        char* msg = "Error: Invalid arguments";
        strcpy(buffer, msg);
        *size = strlen(msg);
    }
    buffer = realloc(buffer, *size+1);
    return buffer;
}   

/**
 * @brief Get the Arguments to execute journalctl
 * 
 * @param jsonString string in json format
 * @param args char** to save the arguments
 */
static void getArguments(char* jsonString, char* args, int isCompressed) {
    cJSON *root = cJSON_Parse(jsonString);
    if(!cJSON_HasObjectItem(root, "arguments")) {
        return;
    }
    cJSON *argsString = cJSON_GetObjectItem(root, "arguments");
    
    char* arguments = cJSON_Print(argsString);
    removeQuotes(arguments);

    strcat(args, "journalctl -o json ");
    strcat(args, arguments);
    if(isCompressed)
        strcat(args, " | gzip - -c -f");

    cJSON_Delete(root);
}

/**
 * @brief Remove quotes from a string
 * 
 * @param str string to be edited
 */
static void removeQuotes(char* str) {
    char *src, *dst;
    dst = src = str;
    while (*src != '\0') {
        if (*src != '"') 
            *dst++ = *src++;
        else
            src++; 
    }
    *dst = '\0';
}

/**
 * @brief Get the response message. This message is the load average and memory free.
 * 
 * @param response char* to save the message
 */
static char* getResponse() {
    FILE *fp;
    char buffer[128];
    cJSON *root = cJSON_CreateObject();
    fp = popen("uptime", "r");
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        cJSON_AddStringToObject(root, "loadAverage", buffer);
    }
    pclose(fp);

    fp = popen("free -h", "r");
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        cJSON_AddStringToObject(root, "memory", buffer);
    }
    pclose(fp);
    char* response = cJSON_Print(root);
    cJSON_Delete(root);
    return response;
}

/**
 * @brief Get the checksum of a message
 * 
 * @param msg char* to calculate checksum
 * @param size size of message
 * @return char* checksum
 */
static void getCheckSum(char* msg, size_t size, char* checksum) {
    pid_t pid;
    int pipeFdIn[2];
    int pipeFdOut[2];

    pipe(pipeFdIn);
    pipe(pipeFdOut);

    pid = fork();

    if(pid==0) {
        close(pipeFdOut[0]);
        close(pipeFdIn[1]);
        dup2(pipeFdIn[0], STDIN_FILENO);
        dup2(pipeFdOut[1], STDOUT_FILENO);
        close(pipeFdIn[0]);
        close(pipeFdOut[1]);
        char* args[] = {"md5sum", NULL};
        execvp(args[0], args);
        perror("Execvp");
        exit(EXIT_FAILURE);
    }
    
    close(pipeFdIn[0]);
	write(pipeFdIn[1], msg, size);
    close(pipeFdIn[1]);
    close(pipeFdOut[1]);
    read(pipeFdOut[0], checksum, 32);
    close(pipeFdOut[0]);
	checksum[32] = '\0';
	waitpid(pid, NULL, 0);
}