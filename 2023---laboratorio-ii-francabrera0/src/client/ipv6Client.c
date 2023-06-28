#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h> 
#include <signal.h>
#include <arpa/inet.h>
#include <bits/sigaction.h>

#include "ipv6Client.h"
#include "clientUtils.h"
#include "macros.h"

static int 	socketFd;
static int initializeSocket(char* address, char* serverPort);
static void sigIntHandler();

/**
 * @brief Create a Ipv6 Client Socket. The client generates a connection to the server, and this 
 * 			remain open until the process is interrupted.
 * 
 * @param address Server address
 * @param port Server port
 */
void createIpv6ClientSocket(char* address, char* port) {

	socketFd = initializeSocket(address, port);
	if(socketFd < 0)
		exit(EXIT_FAILURE);
    
	struct sigaction sa;
	sa.sa_handler = sigIntHandler;
	sigaction(SIGINT, &sa, NULL);
    
	while(1) {
		
		if(sendMessage(socketFd, IPV6) < 0)
			exit(EXIT_FAILURE);

		if(receiveMessage(socketFd, IPV6) < 0)
			exit(EXIT_FAILURE);
	}
}

/**
 * @brief Initialize the socket
 * 
 * @param address Server address
 * @param port Server port
 * @return int On succes, socket file descriptor is returned. On failure, -1 is returned.
 */
static int initializeSocket(char* address, char* serverPort) {
	int port;
	struct sockaddr_in6 serv_addr;
	port = atoi(serverPort);
	
    socketFd = socket(AF_INET6, SOCK_STREAM, 0);
	if(socketFd < 0) {
		perror("Could not create an ipv6 client socket");
		return -1;
	}

	memset((char *) &serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin6_family = AF_INET6;
	inet_pton(AF_INET6, address, &serv_addr.sin6_addr);
	serv_addr.sin6_port = htons((uint16_t) port);

	if (connect(socketFd, (struct sockaddr *)&serv_addr, sizeof(serv_addr ) ) < 0) {
		perror("Could not connect to socket IPv4");
		close(socketFd);
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
	printf(YELLOW"Execution finished\n");
	close(socketFd);
    exit(EXIT_SUCCESS);
}