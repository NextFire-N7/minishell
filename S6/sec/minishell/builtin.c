
#include "builtin.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "process.h"

void jobs(struct process **pl, char **cmd)
{
    struct process *curseur = *pl;
    puts("ID\tPID\tSTATE\tCOMMAND");
    while (curseur != NULL)
    {
        printf("%d\t", curseur->id);
        printf("%d\t", curseur->pid);
        if (curseur->is_running)
        {
            printf("Running\t");
        }
        else
        {
            printf("Stopped\t");
        }
        printf("%s\n", curseur->cmd);
        curseur = curseur->next;
    }
}

void stop(char **cmd) {}

void bg(char **cmd) {}

void fg(char **cmd) {}

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
        stop(cmd);
    }
    else if (!strcmp(cmd[0], "bg"))
    {
        bg(cmd);
    }
    else if (!strcmp(cmd[0], "fg"))
    {
        fg(cmd);
    }
    else
    {
        is_builtin = 0;
    }
    return is_builtin;
}
