
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
        printf("%d\t%d\t%s\t%s\n", curseur->id, curseur->pid, (curseur->is_running) ? "Running" : "Stopped", curseur->cmd);
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
    (*p_to_bg)->is_running = RUNNING;
    printf("[%d] %d: %s &\n", (*p_to_bg)->id, (*p_to_bg)->pid, (*p_to_bg)->cmd);
}

void fg(struct process **pl, char **cmd)
{
    struct process **p_to_bg = (cmd[1]) ? pl_get_id(pl, atoi(cmd[1])) : pl;
    kill((*p_to_bg)->pid, SIGCONT);
    (*p_to_bg)->is_running = RUNNING;
    printf("[%d] %d: %s\n", (*p_to_bg)->id, (*p_to_bg)->pid, (*p_to_bg)->cmd);
    waitpid((*p_to_bg)->pid, NULL, 0);
}

int builtin(struct process **pl, char **cmd)
{
    int is_builtin = 1;
    if (strcmp(cmd[0], "cd") == 0)
    {
        chdir(cmd[1]);
    }
    else if (strcmp(cmd[0], "exit") == 0)
    {
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(cmd[0], "jobs") == 0)
    {
        jobs(pl, cmd);
    }
    else if (strcmp(cmd[0], "stop") == 0)
    {
        stop(pl, cmd);
    }
    else if (strcmp(cmd[0], "bg") == 0)
    {
        bg(pl, cmd);
    }
    else if (strcmp(cmd[0], "fg") == 0)
    {
        fg(pl, cmd);
    }
    else
    {
        is_builtin = 0;
    }
    return is_builtin;
}
