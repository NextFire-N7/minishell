
#include "builtin.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "process.h"

void jobs(struct process **pl, char **cmd)
{
    struct process *curseur = *pl;
    puts("ID\tPID\tSTATE\tCOMMAND");
    while (curseur != NULL)
    {
        printf("%d\t", curseur->id);
        printf("%d\t", curseur->pid);
        printf((curseur->is_running) ? "Running\t" : "Stopped\t");
        printf("%s\n", curseur->cmd);
        curseur = curseur->prec;
    }
}

void stop(struct process **pl, char **cmd)
{
    struct process **p_to_stop = (cmd[1]) ? pl_get_id(pl, atoi(cmd[1])) : pl;
    kill((*p_to_stop)->pid, SIGSTOP);
}

void bg(struct process **pl, char **cmd)
{
    struct process **p_to_bg = (cmd[1]) ? pl_get_id(pl, atoi(cmd[1])) : pl;
    kill((*p_to_bg)->pid, SIGCONT);
    printf("[%d] %d: %s &\n", (*p_to_bg)->id, (*p_to_bg)->pid, (*p_to_bg)->cmd);
}

void fg(struct process **pl, char **cmd)
{
    struct process **p_to_bg = (cmd[1]) ? pl_get_id(pl, atoi(cmd[1])) : pl;
    kill((*p_to_bg)->pid, SIGCONT);
    printf("[%d] %d: %s\n", (*p_to_bg)->id, (*p_to_bg)->pid, (*p_to_bg)->cmd);
    waitpid((*p_to_bg)->pid, NULL, NULL);
}

int builtin(struct process **pl, char **cmd)
{
    int is_builtin = 1;
    if (!strcmp(cmd[0], "cd"))
    {
        chdir(cmd[1]);
    }
    else if (!strcmp(cmd[0], "exit"))
    {
        exit(EXIT_SUCCESS);
    }
    else if (!strcmp(cmd[0], "jobs"))
    {
        jobs(pl, cmd);
    }
    else if (!strcmp(cmd[0], "stop"))
    {
        stop(pl, cmd);
    }
    else if (!strcmp(cmd[0], "bg"))
    {
        bg(pl, cmd);
    }
    else if (!strcmp(cmd[0], "fg"))
    {
        fg(pl, cmd);
    }
    else
    {
        is_builtin = 0;
    }
    return is_builtin;
}
