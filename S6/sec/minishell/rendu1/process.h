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

struct process **pl_get_id(struct process **pl, int id);

struct process **pl_get_pid(struct process **pl, pid_t pid);

#endif
