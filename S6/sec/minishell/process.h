#ifndef PROCESS
#define PROCESS

#include <stdlib.h>

#define RUNNING 1
#define STOPPED 0

struct process
{
    int id;
    pid_t pid;
    int is_running;
    char *cmd;
    struct process *prec;
};

int pl_add(struct process **pl, pid_t pid, char **cmd);

void pl_remove(struct process **pl, pid_t pid);

void pl_set_is_running(struct process **pl, pid_t pid, int is_running);

struct process **pl_get_id(struct process **pl, int id);

#endif
