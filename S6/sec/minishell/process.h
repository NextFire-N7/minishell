#ifndef PROCESS
#define PROCESS

#include <stdlib.h>

struct process
{
    int id;
    pid_t pid;
    int running;
    char *cmd;
    struct process *next;
};

void pl_add(struct process **pl, pid_t pid, char **cmd);

#endif
