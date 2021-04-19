#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include "readcmd.h"

int main(int argc, char const *argv[])
{
    struct cmdline *cmd;
    pid_t pid_fils;
    while (1)
    {
        printf("$ ");
        cmd = readcmd();

        switch (pid_fils = fork())
        {
        case -1:
            perror("fork");
            break;

        case 0:
            execvp(cmd->seq[0][0], cmd->seq[0]);
            printf("%s\n", cmd->err);
            exit(getpid());
            break;
        }
    }

    return 1; // Pas normal si on se retrouve ici...
}
