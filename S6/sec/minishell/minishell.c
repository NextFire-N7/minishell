#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include "readcmd.h"
#include "builtin.h"
#include "processlist.h"

struct process_list *pl;

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
            }
            else if (WIFCONTINUED(etat_fils))
            {
                /* traiter la reprise */
            }
            else if (WIFEXITED(etat_fils))
            {
                /* traiter exit */
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
    pl = malloc(sizeof(struct process_list));

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
        if (!builtin(cmd->seq[0], pl))
        {
            switch (pid_fils = fork())
            {
            case -1:
                puts("ECHEC fork");
                break;
            case 0:
                execvp(cmd->seq[0][0], cmd->seq[0]);
                printf("%s\n", cmd->err);
                exit(getpid());
                break;
            default:
                pl_add(pl, pid_fils, cmd->seq[0]);
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
