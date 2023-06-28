#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include "macros.h"

int sendMessage(int socketFd, CLIENTS client);
int receiveMessage(int socketFd, CLIENTS client);
char* getCheckSum(char* msg, size_t size);



#endif /*CLIENT_UTILS_H*/