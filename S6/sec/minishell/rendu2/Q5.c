#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include "readcmd.h"
#include "builtin.h"

int main(int argc, char const *argv[])
{
    struct cmdline *cmd;
    char cwd[1024];
    pid_t pid_fils;
    while (1)
    {
        getcwd(cwd, sizeof(cwd));
        printf("%s$ ", cwd);
        cmd = readcmd();

        if (!builtin(NULL, cmd->seq[0], NULL))
        {
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

            default:
                if (cmd->backgrounded)
                {
                    printf("%d backgrounded.\n", pid_fils);
                }
                else
                {
                    wait(NULL);
                }
                break;
            }
        }
    }

    return 1; // Pas normal si on se retrouve ici...
}
