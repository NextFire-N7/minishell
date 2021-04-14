#include <stdlib.h>
#include <string.h>
#include "processlist.h"

char *cmd_to_string(char **cmd)
{
    return NULL;
}

void pl_add(struct process_list *pl, pid_t pid, char **cmd)
{
    struct process_list *p = malloc(sizeof(struct process_list));
    p->pid = pid;
    p->running = 1;
    p->cmd = cmd_to_string(cmd);
    p->next = NULL;
    if (!pl)
    {
        p->id = 1;
        pl = p;
    }
    else
    {
        struct process_list *cursor = pl;
        while (cursor->next)
        {
            cursor = cursor->next;
        }
        p->id = cursor->id + 1;
        cursor->next = p;
    }
}