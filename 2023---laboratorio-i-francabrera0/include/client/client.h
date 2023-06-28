#ifndef CLIENT_H
#define CLIENT_H

#include "../utilities/msgStructs.h"
#include "../utilities/semStructs.h"

char* sendRequest(MessageQueue requestQueue, Semaphore semaphoreRequestQueue, long type, const char* procName);
int initializeSemaphore(Semaphore* semaphore, char* path);
int semaphoreAcquire(Semaphore semaphore);
int semaphoreRelease(Semaphore semaphore);

#endif /*CLIENT_H*/