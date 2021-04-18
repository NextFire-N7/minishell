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

int pl_add(struct process **pl, pid_t pid, char **cmd)
{
    struct process *p = malloc(sizeof(struct process));
    p->pid = pid;
    p->is_running = RUNNING;
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
    return p->id;
}

void pl_remove(struct process **pl, pid_t pid)
{
    if (!(*pl)->next)
    {
        free(*pl);
        *pl = NULL;
    }
    else
    {
        if ((*pl)->next->pid != pid)
        {
            pl_remove(&(*pl)->next, pid);
        }
        struct process *to_free = (*pl)->next;
        (*pl)->next = to_free->next;
        free(to_free);
    }
}

void pl_set_is_running(struct process **pl, pid_t pid, int is_running)
{
    if ((*pl)->pid != pid)
    {
        pl_set_is_running(&(*pl)->next, pid, is_running);
    }
    (*pl)->is_running = is_running;
}
