#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cjson/cJSON.h>

#include "clientUtils.h"

static char* getMessageToSend(CLIENTS client);
static int getFileName(char* fileName);
static int createFile(char* buffer, size_t bufferSize);



/**
 * @brief Send a message to the server. The content of the message is obtained by the function getMessageToSend()
 * 
 * @param socketFd Socket file descriptor 
 * @return int On success, 0 is returned. On failure, -1 is returned
 */
int sendMessage(int socketFd, CLIENTS client) {
	char buffer[BUFFSIZE];
	memset(buffer, '\0', BUFFSIZE);
    strcpy(buffer, getMessageToSend(client));

	char* checkSum = getCheckSum(buffer, strlen(buffer));
	strcat(buffer, checkSum);
	free(checkSum);
		
	ssize_t size = write( socketFd, buffer, sizeof(buffer));
	if ( size < 0 ) {
		perror("Could not write in socket");
		close(socketFd);
		return -1;
	}
	return 0;
}

/**
 * @brief Get the message to send to the Server from the arguments entered by the user.
 * 			Format {"arguments": "arguments entered by the user"}
 * 
 * @return char* Message to send
 */
static char* getMessageToSend(CLIENTS client) {
	char buffer[BUFFSIZE];
    switch (client)
    {
    case IPV4:
        printf(BLUE"Please, enter the arguments to journalctl: ");
        break;
    case IPV6:
        printf(YELLOW"Please, enter the arguments to journalctl: ");
        break;
    case UNIX:
        return "{GET REPORT}";
        break;
    default:
        break;
    }
	memset(buffer, '\0', BUFFSIZE);
	fgets(buffer, BUFFSIZE-1, stdin);
	buffer[strcspn(buffer, "\n")] = '\0';
	
	cJSON *root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "arguments", buffer);
	
    char *results = cJSON_Print(root);
    cJSON_Delete(root);
    
	return results;
}

/**
 * @brief Received a message from server. The content of the message is printed.
 * 
 * @param socketFd Socket file descriptor
 * @return int On success, 0 is returned. On failure, -1 is returned
 */
int receiveMessage(int socketFd, CLIENTS client) {
	ssize_t bytesRead;
	size_t  size;
	size_t bufferSize = BUFFSIZE;

	char* buffer = malloc(bufferSize);
	memset(buffer, '\0', bufferSize);
	if(!buffer) {
		perror("Could not allocate memory");
		return -1;
	}
	while((bytesRead = read(socketFd, buffer + size, BUFFSIZE)) > 0) {
		size += (size_t)bytesRead;
		if(size >= bufferSize) {
			bufferSize += BUFFSIZE;
			buffer = realloc(buffer, bufferSize);
			memset(buffer+size, '\0', BUFFSIZE);
			if(!buffer) {
				perror("Could not allocate memory");
				free(buffer);
				return -1;
			}	
		}
		else
			break;
	}

	//Test checksum - I don't like how it is, but it works
	size_t separator = size-33;
	char message[separator+1];
	for(size_t i=0; i<separator; i++)
		*(message+i) = *(buffer+i);
	message[separator] = '\0';
	char checksum[33];
	strncpy(checksum, buffer+separator, 33);
	
	free(buffer);

	if(strcmp(checksum, getCheckSum(message, separator))){
        perror("Checksum does not match");
        return -1;
    }
	///

    if(client == IPV6)
	    createFile(message, bufferSize);
	
	printf(RESET_COLOR"Response from server:\n");
    printf(RESET_COLOR"%s\n", message);
	printf(RESET_COLOR"%ld\n\n", size);
	return 0;
}

/**
 * @brief Create a File to save compressed data
 * 
 * @param buffer data to save
 * @param bufferSize size of data
 * @return int On success 0 is returned. On failure -1 is returned.
 */
static int createFile(char* buffer, size_t bufferSize) {
	char* fileName = (char*)malloc(sizeof(char) * 32);
	if((getFileName(fileName)) < 0) {
		free(fileName);
		return -1;
	}
	FILE* file = fopen(fileName, "w");
	if(file == NULL) {
		perror("Could not open the file");
		free(fileName);
		return -1;
	}
	size_t bytesSaved = fwrite(buffer, sizeof(char), bufferSize, file);
	if(bytesSaved < bufferSize) {
		perror("File write failure");
		free(fileName);
		fclose(file);
		return -1;
	}

	fclose(file);
	free(fileName);
	return 0;
}

/**
 * @brief Get the file name to save compressed data
 * 
 * @param fileName char * to save the generated name
 * @return int On success 0 is returned. On failure -1 is returned
 */
static int getFileName(char* fileName) {

	if(access(TMPDIR, F_OK) != 0) {
		if(mkdir(TMPDIR, 0777) <0) {
			perror("Could not create tmp directory");
			return -1;
		}
	}
	time_t timeStamp = time(NULL);
	if((sprintf(fileName, "%soutput%ld.gz", TMPDIR, timeStamp)) < 0) {
		perror("Could not create file name");
		return -1;
	}
	return 0;
}

/**
 * @brief Get the checksum of a message
 * 
 * @param msg char* to calculate checksum
 * @param size size of message
 * @return char* checksum
 */
char* getCheckSum(char* msg, size_t size) {
    
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
    char* md5 = malloc(33);
    read(pipeFdOut[0], md5, 32);
    close(pipeFdOut[0]);
	*(md5+32) = '\0';
	waitpid(pid, NULL, 0);
    return md5;
}