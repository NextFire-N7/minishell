#include "process.h"
#include <stdlib.h>
#include <string.h>

char *cmd_to_string(char **cmd)
{
    char *string = NULL;
    for (int i = 0; cmd[i] != 0; i++)
    {
        if (string)
        {
            strcat(string, " ");
        }
        string = realloc(string, (strlen(cmd[i]) + 1) * sizeof(char));
        strcat(string, cmd[i]);
    }
    return string;
}

void pl_add(struct process **pl, pid_t pid, char **cmd)
{
    struct process *p = malloc(sizeof(struct process));
    p->pid = pid;
    p->running = 1;
    p->cmd = cmd_to_string(cmd);
    p->next = NULL;
    if (!*pl)
    {
        p->id = 1;
        *pl = p;
    }
    else
    {
        struct process *cursor = *pl;
        while (cursor->next)
        {
            cursor = cursor->next;
        }
        p->id = cursor->id + 1;
        cursor->next = p;
    }
}
