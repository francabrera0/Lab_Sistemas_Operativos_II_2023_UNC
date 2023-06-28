#ifndef MACROS_H
#define MACROS_H

typedef enum {
    IPV4,
    IPV6,
    UNIX
} CLIENTS;

#define BUFFSIZE    1024
#define TMPDIR      "./tmp/"

#define RESET_COLOR	"\x1b[0m"
#define RED         "\x1b[31m"
#define GREEN		"\x1b[32m"
#define YELLOW      "\x1b[33m"
#define BLUE		"\x1b[36m"
#define WHITE   	"\x1b[37m"

#endif /*MACROS_H*/