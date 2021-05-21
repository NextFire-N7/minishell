#include "builtin.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "process.h"

static void jobs(struct process **pl, char **cmd)
{
    if (!*pl)
    {
        fprintf(stderr, "jobs: There are no jobs\n");
        return;
    }
    struct process *curseur = *pl;
    puts("ID\tPID\tSTATE\tCOMMAND");
    while (curseur)
    {
        printf("%d\t%d\t%s\t%s\n", curseur->id, curseur->pid, (curseur->is_running) ? "Running" : "Stopped", curseur->cmd);
        curseur = curseur->prec;
    }
}

static void stop(struct process **pl, char **cmd)
{
    struct process **p_to_stop = (cmd[1]) ? pl_get_by_id(pl, atoi(cmd[1])) : pl;
    if (!*p_to_stop)
    {
        fprintf(stderr, "fg: There are no suitable jobs\n");
        return;
    }
    kill((*p_to_stop)->pid, SIGSTOP);
}

static pid_t cont(struct process **pl, char **cmd)
{
    struct process **p = (cmd[1]) ? pl_get_by_id(pl, atoi(cmd[1])) : pl;
    if (!*p)
    {
        fprintf(stderr, "fg/bg: There are no suitable jobs\n");
        return -1;
    }
    kill((*p)->pid, SIGCONT);
    (*p)->is_running = RUNNING;
    printf("[%d] %d: %s\n", (*p)->id, (*p)->pid, (*p)->cmd);
    return (*p)->pid;
}

static void bg(struct process **pl, char **cmd)
{
    cont(pl, cmd);
}

static void fg(struct process **pl, char **cmd, pid_t *pid_bg)
{
    pid_t pid = cont(pl, cmd);
    if (pid != -1)
    {
        *pid_bg = pid;
        pause();
    }
}

int builtin(struct process **pl, char **cmd, pid_t *pid_bg)
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
        fg(pl, cmd, pid_bg);
    }
    else
    {
        is_builtin = 0;
    }
    return is_builtin;
}
