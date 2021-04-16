#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include "readcmd.h"
#include "builtin.h"
#include "process.h"

struct process *pl = NULL;

void suivi_fils(int sig)
{
    int etat_fils, pid_fils;
    do
    {
        pid_fils = (int)waitpid(-1, &etat_fils, WNOHANG | WUNTRACED | WCONTINUED);
        if ((pid_fils == -1) && (errno != ECHILD))
        {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        else if (pid_fils > 0)
        {
            if (WIFSTOPPED(etat_fils))
            {
                /* traiter la suspension */
                pl_set_is_running(&pl, pid_fils, SUSPENDED);
            }
            else if (WIFCONTINUED(etat_fils))
            {
                /* traiter la reprise */
                pl_set_is_running(&pl, pid_fils, RUNNING);
            }
            else if (WIFEXITED(etat_fils))
            {
                /* traiter exit */
                pl_remove(&pl, pid_fils);
            }
            else if (WIFSIGNALED(etat_fils))
            {
                /* traiter signal */
            }
        }
    } while (pid_fils > 0);
    /* autres actions après le suivi des changements d'état */
}

int main(int argc, char const *argv[])
{
    struct sigaction handler_sigchld;
    handler_sigchld.sa_handler = suivi_fils;
    sigaction(SIGCHLD, &handler_sigchld, NULL);

    struct cmdline *cmd;
    char cwd[1024];
    pid_t pid_fils;
    while (1)
    {
        getcwd(cwd, sizeof(cwd));
        printf("%s$ ", cwd);
        cmd = readcmd();
        if (!builtin(&pl, cmd->seq[0]))
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
                pl_add(&pl, pid_fils, cmd->seq[0]);
                if (!cmd->backgrounded)
                {
                    wait(NULL);
                }
                break;
            }
        }
    }

    return 1; // Pas normal si on se retrouve ici...
}
