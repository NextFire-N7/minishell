#include "process.h"

#include <string.h>

static int id = 1;

// copie la cmd sous forme de tableau en une unique chaine
static char *cmd_to_string(char **cmd)
{
    char *string = malloc((strlen(cmd[0]) + 1) * sizeof(char));
    strcpy(string, cmd[0]);
    for (int i = 1; cmd[i]; i++)
    {
        string = realloc(string, (strlen(cmd[i]) + 2) * sizeof(char));
        strcat(strcat(string, " "), cmd[i]);
    }
    return string;
}

int pl_add(struct process **pl, pid_t pid, char **cmd)
{
    struct process *p = malloc(sizeof(struct process));
    p->id = id++;
    p->pid = pid;
    p->is_running = RUNNING;
    p->cmd = cmd_to_string(cmd);
    p->prec = *pl;
    *pl = p;
    return p->id;
}

void pl_remove(struct process **pl, pid_t pid)
{
    if ((*pl)->pid == pid) // celui à retirer est en tête
    {
        struct process *prec = (*pl)->prec;
        free(*pl);
        *pl = prec;
    }
    else // il est qque part au milieu
    {
        if ((*pl)->prec->pid != pid)
        {
            pl_remove(&(*pl)->prec, pid);
        }
        struct process *to_free = (*pl)->prec;
        (*pl)->prec = to_free->prec;
        free(to_free);
    }
}

struct process **pl_get_by_id(struct process **pl, int id)
{
    if (!*pl || (*pl)->id == id)
    {
        return pl;
    }
    return pl_get_by_id(&(*pl)->prec, id);
}

struct process **pl_get_by_pid(struct process **pl, pid_t pid)
{
    if (!*pl || (*pl)->pid == pid)
    {
        return pl;
    }
    return pl_get_by_pid(&(*pl)->prec, pid);
}
