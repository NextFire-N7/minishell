#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "readcmd.h"

int main(int argc, char const *argv[])
{
    struct cmdline *cmd;
    char cwd[1024];

    while (1)
    {
        getcwd(cwd, sizeof(cwd));
        printf("%s $ ", cwd);
        cmd = readcmd();

        if (!strcmp(cmd->seq[0][0], "cd"))
        {
            chdir(cmd->seq[0][1]);
        }
        else if (!strcmp(cmd->seq[0][0], "exit"))
        {
            break;
        }
        else
        {
            switch (fork())
            {
            case -1:
                printf("ECHEC fork\n");
                break;
            case 0:
                execvp(cmd->seq[0][0], cmd->seq[0]);
                printf("%s\n", cmd->err);
                return getpid();
                break;
            default:
                if (!cmd->backgrounded)
                {
                    wait(NULL);
                }
                break;
            }
        }
    }

    return 0;
}
