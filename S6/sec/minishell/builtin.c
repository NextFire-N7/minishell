
#include "builtin.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "process.h"

void jobs(char **cmd) {}

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
        jobs(cmd);
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
