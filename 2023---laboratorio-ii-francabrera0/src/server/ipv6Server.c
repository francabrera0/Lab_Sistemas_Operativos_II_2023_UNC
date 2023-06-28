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


#include "ipv6Server.h"
#include "serverUtils.h"
#include "macros.h"

static int initializeSocket(char* serverPort);
static void sigChldHandler();

/**
 * @brief Create a Ipv6 Server Socket
 * 
 * @param serverPort Server port
 */
void createIpv6ServerSocket(char* serverPort) {

    int socketFd, 
        newSocketFd;

    socketFd = initializeSocket(serverPort);
    if(socketFd < 0) {
        exit(EXIT_FAILURE);
    }

    socklen_t clientAddressLen;
	struct sockaddr_in6 cli_addr;
	clientAddressLen = sizeof(cli_addr);

    struct sigaction sa;
    sa.sa_handler = sigChldHandler;
    sigaction(SIGCHLD, &sa, NULL);

    int isCompressed = 1;
	while(1) {
		newSocketFd = accept(socketFd, (struct sockaddr *) &cli_addr, &clientAddressLen);

        pid_t pid;
		if((pid = fork()) < 0) {
            perror(RESET_COLOR"Could not create child for IPv6 server");
            close(newSocketFd);
            exit(EXIT_FAILURE);
        }

		if(pid == 0) {
		    close(socketFd);
		    while (1) {
                char* args = (char*)malloc(sizeof(char) * 64);
                memset(args, '\0', 64);

                if(receiveMessage(newSocketFd, args, IPV6, isCompressed) < 0)
                    exit(EXIT_FAILURE);
                
                if(sendMessage(newSocketFd, args, IPV6) < 0)
                    exit(EXIT_FAILURE);

                free(args);
		    }
		}
		else {
		  printf(YELLOW"IPv6 SERVER: New client, in charge of process child: %d\n", pid);
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
    
    int socketFd = socket(AF_INET6, SOCK_STREAM, 0);

    if(socketFd < 0) {
        perror(RESET_COLOR"Could not create ipv6 socket");
        return -1;
    }
    struct sockaddr_in6 serv_addr;
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	int port = atoi(serverPort);
	serv_addr.sin6_family = AF_INET6;
	serv_addr.sin6_addr = in6addr_any;
	serv_addr.sin6_port = htons((uint16_t) port);

	if(bind(socketFd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror(RESET_COLOR"Binding error");
        close(socketFd);
		return -1;
	}

    printf(YELLOW"Process: %d - socket available in port: %d IPV6\n", getpid(), ntohs(serv_addr.sin6_port));
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