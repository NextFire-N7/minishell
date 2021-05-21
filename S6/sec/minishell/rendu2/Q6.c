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
pid_t pid_fils;

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
                struct process **p_stopped = pl_get_by_pid(&pl, pid_fils);
                (*p_stopped)->is_running = STOPPED;
                printf("[%d] %d: %s — Stopped\n", (*p_stopped)->id, (*p_stopped)->pid, (*p_stopped)->cmd);
            }
            else if (WIFCONTINUED(etat_fils))
            {
                /* traiter la reprise */
                struct process **p_started = pl_get_by_pid(&pl, pid_fils);
                (*p_started)->is_running = RUNNING;
                printf("[%d] %d: %s — Running\n", (*p_started)->id, (*p_started)->pid, (*p_started)->cmd);
            }
            else if (WIFEXITED(etat_fils))
            {
                /* traiter exit */
                pl_remove(&pl, pid_fils);
            }
            else if (WIFSIGNALED(etat_fils))
            {
                /* traiter signal */
                pl_remove(&pl, pid_fils);
            }
        }
    } while (pid_fils > 0);
    /* autres actions après le suivi des changements d'état */
}

void fwd_sig(int sig)
{
    kill(pid_fils, sig);
}

int main(int argc, char const *argv[])
{
    struct sigaction handler_sigchld;
    handler_sigchld.sa_handler = suivi_fils;

    struct sigaction handler_fwd_sig;
    handler_fwd_sig.sa_handler = fwd_sig;
    sigaction(SIGTSTP, &handler_fwd_sig, NULL);
    sigaction(SIGSTOP, &handler_fwd_sig, NULL);

    struct cmdline *cmd;
    char cwd[1024];
    while (1)
    {
        getcwd(cwd, sizeof(cwd));
        printf("%s$ ", cwd);
        do
        {
            cmd = readcmd();
        } while (!cmd || !cmd->seq[0]);

        if (!builtin(&pl, cmd->seq[0], &pid_fils))
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
            {
                sigaction(SIGCHLD, &handler_sigchld, NULL);
                int id = pl_add(&pl, pid_fils, cmd->seq[0]);
                if (cmd->backgrounded)
                {
                    printf("[%d] %d\n", id, pid_fils);
                }
                else
                {
                    waitpid(pid_fils, NULL, 0);
                }
                break;
            }
            }
        }
    }

    return 1; // Pas normal si on se retrouve ici...
}
