#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include "macros.h"

int receiveMessage(int socketFd, char* args, CLIENTS client, int isCompressed);
int sendMessage(int socketFd, char* args, CLIENTS client);


#endif /*SERVER_UTILS_H*/