#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "readcmd.h"

int main(int argc, char const *argv[])
{
    struct cmdline *cmd;
    pid_t status;

    while (1)
    {
        printf(">>> ");
        cmd = readcmd();

        switch (fork())
        {
        case -1:
            printf("ECHEC fork\n");
            break;
        case 0:
            execvp(cmd->seq[0][0], cmd->seq[0]);
            printf("ECHEC exec\n");
            break;
        default:
            wait(&status);
            break;
        }
    }

    return 0;
}
