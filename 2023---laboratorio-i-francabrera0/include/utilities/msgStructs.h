#ifndef MSG_STRUCTS_H
#define MSG_STRUCTS_H

#include <sys/msg.h>

typedef struct message {
    long type;
    char text[60];
} Message;

typedef struct messageQueue {
    key_t key;
    int id;
    Message msg;
} MessageQueue;


#endif /*MSG_STRUCTS_H*/
