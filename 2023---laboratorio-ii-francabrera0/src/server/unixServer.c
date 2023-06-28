#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <bits/sigaction.h>

#include "unixServer.h"
#include "serverUtils.h"
#include "macros.h"

static int initializeSocket(char* address);
static void sigChldHandler();

/**
 * @brief Create a Unix Server Socket 
 * 
 * @param address server address
 */
void createUnixServerSocket(char* address) {

    int socketFd, 
        newSocketFd;

    socketFd = initializeSocket(address);
    if(socketFd < 0)
        exit(EXIT_FAILURE);

    struct sockaddr_un  cli_addr;
    socklen_t clientAddressLen;
    clientAddressLen = sizeof(cli_addr);

    struct sigaction sa;
    sa.sa_handler = sigChldHandler;
    sigaction(SIGCHLD, &sa, NULL);

    while (1) {
        newSocketFd = accept(socketFd, (struct sockaddr *) &cli_addr, &clientAddressLen);
            
        pid_t pid;
        if((pid = fork()) < 0) {
            perror(RESET_COLOR"Could not create child for unix server");
            close(newSocketFd);
            exit(EXIT_FAILURE);
        }
      
        if( pid == 0 ) {
            close(socketFd);
            while(1) { 
                
                if(receiveMessage(newSocketFd, NULL, UNIX, 0) < 0)
                    exit(EXIT_FAILURE);

                if(sendMessage(newSocketFd, NULL, UNIX) < 0)
                    exit(EXIT_FAILURE);
            }
        }
        else {
	        printf(GREEN"Unix SERVER: New client, in charge of process child: %d\n", pid );
	        close(newSocketFd);
        }
    }
}

/**
 * @brief Initialize the socket
 * 
 * @param address Socket address
 * @return int On success socket file descriptor is returned. On failure -1 is returned
 */
static int initializeSocket(char* address) {
    int socketFd = socket(AF_UNIX, SOCK_STREAM, 0);

    if(socketFd < 0) {
        perror(RESET_COLOR"Could not create unix socket");
        return -1;
    }
  
    unlink(address);
    struct sockaddr_un  serv_addr; 
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, address);
    socklen_t serverAddressLen = sizeof(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
  
    if(bind(socketFd, (struct sockaddr *) &serv_addr, serverAddressLen) < 0) {
        perror(RESET_COLOR"Binding error");
        close(socketFd);
        return -1;
    }
    
    printf(GREEN"Process: %d - socket available in sokcet: %s UNIX\n", getpid(), serv_addr.sun_path );
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