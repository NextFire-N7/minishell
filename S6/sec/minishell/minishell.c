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
int prompt;

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
            printf("suivi_fils %d\n", pid_fils);
            if (WIFSTOPPED(etat_fils))
            {
                /* traiter la suspension */
                struct process **p_stopped = pl_get_pid(&pl, pid_fils);
                (*p_stopped)->is_running = STOPPED;
                printf("[%d] %d: %s — Stopped\n", (*p_stopped)->id, (*p_stopped)->pid, (*p_stopped)->cmd);
            }
            else if (WIFCONTINUED(etat_fils))
            {
                /* traiter la reprise */
                struct process **p_started = pl_get_pid(&pl, pid_fils);
                (*p_started)->is_running = RUNNING;
                printf("[%d] %d: %s\n", (*p_started)->id, (*p_started)->pid, (*p_started)->cmd);
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

void fwd_sig_stop(int sig)
{
    if (!prompt)
    {
        printf("fwd_sig_stop %d\n", pid_fils);
        kill(pid_fils, SIGSTOP);
    }
}

void fwd_sig_kill(int sig)
{
    if (!prompt)
    {
        printf("fwd_sig_kill %d\n", pid_fils);
        kill(pid_fils, SIGKILL);
    }
}

int main(int argc, char const *argv[])
{
    struct sigaction handler_sigchld;
    handler_sigchld.sa_handler = suivi_fils;
    struct sigaction handler_fwd_stop;
    handler_fwd_stop.sa_handler = fwd_sig_stop;
    struct sigaction handler_fwd_kill;
    handler_fwd_kill.sa_handler = fwd_sig_kill;
    struct sigaction handler_mask;
    handler_mask.sa_handler = SIG_IGN;

    sigaction(SIGCHLD, &handler_sigchld, NULL);
    sigaction(SIGTSTP, &handler_fwd_stop, NULL);
    sigaction(SIGINT, &handler_fwd_kill, NULL);

    int id;
    struct cmdline *cmd;
    char cwd[1024];
    while (1)
    {
        getcwd(cwd, sizeof(cwd));
        printf("%s$ ", cwd);
        prompt = 1;
        do
        {
            cmd = readcmd();
        } while (!cmd || !cmd->seq[0]);
        prompt = 0;

        if (!builtin(&pl, cmd->seq[0]))
        {
            switch (pid_fils = fork())
            {
            case -1:
                perror("fork");
                break;

            case 0:
                sigaction(SIGTSTP, &handler_mask, NULL);
                sigaction(SIGINT, &handler_mask, NULL);
                execvp(cmd->seq[0][0], cmd->seq[0]);
                printf("%s\n", cmd->err);
                exit(getpid());
                break;

            default:
            {
                id = pl_add(&pl, pid_fils, cmd->seq[0]);
                if (cmd->backgrounded)
                {
                    printf("[%d] %d\n", id, pid_fils);
                }
                else
                {
                    pause();
                }
                break;
            }
            }
        }
    }

    return 1; // Pas normal si on se retrouve ici...
}
