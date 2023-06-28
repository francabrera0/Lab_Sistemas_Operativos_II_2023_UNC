#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

void sigIntHandler(int sig) {
    printf("Proceso %d interrumpido.\n", getpid());
    exit(0);
}

int main(int argc, char const *argv[])
{
    if(argc < 2) {
        perror("Please enter the number of clients");
        exit(EXIT_FAILURE);
    }

    int clientNumber = atoi(argv[1]);
    if(clientNumber == 0) {
        perror("Invalid argument");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, sigIntHandler);


    pid_t pid;

    for(int i=0; i<clientNumber; i++) {
        pid = fork();
        if(pid < 0) {
            perror("Could not create a child");
            exit(EXIT_FAILURE);
        }
        else if(pid == 0) {
            char* args[] = {"../build/Client", "unix", "../sock", NULL};

            execvp(args[0], args);
            perror("Execvp");
            exit(EXIT_FAILURE);
        }
    }

    sleep(10);

    for(int i=0; i < clientNumber; i++) {
        kill(0, SIGINT);
    }

    int status;
    pid_t wpid;
    while((wpid = wait(&status)) > 0);

    return 0;
}
