#ifndef SHM_STRUCTS_H
#define SHM_STRUCTS_H

#include <sys/msg.h>

typedef struct shmemory {
    key_t key;
    int id;
    void* shmAddress;
} ShMemory;


#endif /*MSG_STRUCTS_H*/
