#ifndef SEM_STRUCTS_H
#define SEM_STRUCTS_H

#include <sys/msg.h>

typedef struct semaphore {
    key_t key;
    int id;
} Semaphore;


#endif /*MSG_STRUCTS_H*/
