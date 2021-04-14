#ifndef PROCESSLIST
#define PROCESSLIST

#include <stdlib.h>

struct process_list
{
    int id;
    pid_t pid;
    int running;
    char *cmd;
    struct process_list *next;
};

void pl_add(struct process_list *pl, pid_t pid, char **cmd);

void pl_free(struct process_list *pl);

#endif
