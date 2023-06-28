#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <bits/sigaction.h>

#include "unixClient.h"
#include "clientUtils.h"
#include "macros.h"

static int 	socketFd;

static int initializeSocket(char* address);
static void sigIntHandler();

/**
 * @brief Create a Unix Client Socket. The client generates a connection to the server, and this 
 * 			remain open until the process is interrupted.
 * 
 * @param argv Server address 
 */
void createUnixClientSocket(char* address) {

    socketFd = initializeSocket(address);
	if(socketFd < 0)
		exit(EXIT_FAILURE);

	struct sigaction sa;
	sa.sa_handler = sigIntHandler;
	sigaction(SIGINT, &sa, NULL);
	
	while(1) {
		sleep(4);
		
		if(sendMessage(socketFd, UNIX) < 0)
			exit(EXIT_FAILURE);

        if(receiveMessage(socketFd, UNIX) < 0)
			exit(EXIT_FAILURE);

	}
}

/**
 * @brief Initialize the socket
 * 
 * @param address Server address
 * @return int On success, socket file descriptor is returned. On failure, -1 is returned
 */
static int initializeSocket(char* address) {

    socklen_t serverAddressLen;
	struct sockaddr_un serv_addr;
    
    socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(socketFd < 0) {
		perror("Could not create an unix client socket");
		return -1;
	}

	memset((char *)&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, address);
	serverAddressLen = sizeof(serv_addr.sun_path) + sizeof(serv_addr.sun_family);

 	if (connect(socketFd, (struct sockaddr *)&serv_addr, serverAddressLen) < 0) {
		perror("Could not connect to socket unix");
		return -1;
	}
	return socketFd;

}

/**
 * @brief Handler for SIGINT signal, send 'end' to server.
 * 
 */
static void sigIntHandler() {
	char endMsg[128] = "end";
	char* checkSum = getCheckSum(endMsg, strlen(endMsg));
	strcat(endMsg,checkSum);
	free(checkSum);
	if((write(socketFd, endMsg, strlen(endMsg))) < 0) {
		perror("Could not write in socket");
		close(socketFd);
		exit(EXIT_FAILURE);
	}
	printf(GREEN"Execution finished\n");
	close(socketFd);
    exit(EXIT_SUCCESS);
}