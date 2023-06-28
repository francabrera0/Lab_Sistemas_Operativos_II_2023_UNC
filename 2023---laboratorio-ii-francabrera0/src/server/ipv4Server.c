#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>
#include <bits/sigaction.h>


#include "ipv4Server.h"
#include "serverUtils.h"
#include "macros.h"

static int initializeSocket(char* serverPort);
static void sigChldHandler();

/**
 * @brief Create a Ipv4 Server Socket
 * 
 * @param serverPort Server port
 */
void createIpv4ServerSocket(char* serverPort) {

    int socketFd, 
        newSocketFd;

    socketFd = initializeSocket(serverPort);
    if(socketFd < 0) {
        exit(EXIT_FAILURE);
    }

	struct sockaddr_in cli_addr;
    socklen_t clientAddressLen;
	clientAddressLen = sizeof(cli_addr);

    struct sigaction sa;
    sa.sa_handler = sigChldHandler;
    sigaction(SIGCHLD, &sa, NULL);

    int isCompressed = 0;
	while(1) {
		newSocketFd = accept(socketFd, (struct sockaddr *) &cli_addr, &clientAddressLen);

        pid_t pid;
		if((pid = fork()) < 0) {
            perror("Could not create child for IPv4 server");
            close(newSocketFd);
            exit(EXIT_FAILURE);
        }

		if(pid == 0) {
		    close(socketFd);
		    while (1) {

                char* args = (char*)malloc(sizeof(char) * 64);
                memset(args, '\0', 64);

                if(receiveMessage(newSocketFd, args, IPV4, isCompressed) < 0)
                    exit(EXIT_FAILURE);
                if(sendMessage(newSocketFd, args, IPV4) < 0)
                    exit(EXIT_FAILURE);

                free(args);
		    }
		}
		else {
		  printf(BLUE"IPv4 SERVER: New client, in charge of process child: %d\n", pid);
		  close(newSocketFd);
		}
	}
}

/**
 * @brief Initialize the socket
 * 
 * @param serverPort Server port
 * @return int On success, socket file descriptor is returned. On failure, -1 is returned
 */
static int initializeSocket(char* serverPort) {
    int socketFd = socket(AF_INET, SOCK_STREAM, 0);

    if(socketFd < 0) {
        perror("Could not create ipv4 socket");
        return -1;
    }
    struct sockaddr_in  serv_addr;
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	int port = atoi(serverPort);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons((uint16_t) port);

	if(bind(socketFd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("Binding error");
		close(socketFd);
        return -1;
	}

    printf(BLUE"Process: %d - socket available in port: %d IPV4\n", getpid(), ntohs(serv_addr.sin_port));
	listen(socketFd, 5);
    return socketFd;
}

/**
 * @brief Handler to SIGCHLD signal
 * 
 */
static void sigChldHandler() {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    }
}