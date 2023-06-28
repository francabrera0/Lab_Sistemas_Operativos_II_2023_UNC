#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <bits/sigaction.h>
#include <sys/wait.h>

#include "server.h"
#include "ipv4Server.h"
#include "ipv6Server.h"
#include "unixServer.h"


pid_t createChild();

int main(int argc, char *argv[]) {

    if(argc < 4) {
        perror("Invalid arguments, please specify <ipv4 port> <ipv6 port> <file unixipc>");
        exit(EXIT_FAILURE);
    }

    pid_t pid = createChild();
    if(pid == 0) {
        createIpv4ServerSocket(argv[1]);
        return(EXIT_SUCCESS);
    }

    pid = createChild();
    if(pid == 0) {
        createIpv6ServerSocket(argv[2]);
        return(EXIT_SUCCESS);
    }

    pid = createChild();
    if(pid == 0) {
        createUnixServerSocket(argv[3]);
        return(EXIT_SUCCESS);
    }

    pid_t wpid;
    while((wpid = wait(NULL)) > 0);

    return (EXIT_SUCCESS);

}

pid_t createChild() {
    pid_t pid;
    if((pid = fork()) < 0) {
        perror("Could not create a child");
        exit(EXIT_FAILURE);
    }
    return pid;
}

