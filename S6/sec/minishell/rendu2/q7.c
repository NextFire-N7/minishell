#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include "readcmd.h"
#include "builtin.h"
#include "process.h"

struct process *pl = NULL;
pid_t pid_fils;
int in_prompt;

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
                // printf("WIFSTOPPED %d\n", pid_fils);
                /* traiter la suspension */
                struct process **p_stopped = pl_get_pid(&pl, pid_fils);
                (*p_stopped)->is_running = STOPPED;
                printf("[%d] %d: %s — Stopped\n", (*p_stopped)->id, (*p_stopped)->pid, (*p_stopped)->cmd);
            }
            else if (WIFCONTINUED(etat_fils))
            {
                // printf("WIFCONTINUED %d\n", pid_fils);
                /* traiter la reprise */
                struct process **p_started = pl_get_pid(&pl, pid_fils);
                (*p_started)->is_running = RUNNING;
                printf("[%d] %d: %s\n", (*p_started)->id, (*p_started)->pid, (*p_started)->cmd);
            }
            else if (WIFEXITED(etat_fils))
            {
                // printf("WIFEXITED %d\n", pid_fils);
                /* traiter exit */
                pl_remove(&pl, pid_fils);
            }
            else if (WIFSIGNALED(etat_fils))
            {
                // printf("WIFSIGNALED %d\n", pid_fils);
                /* traiter signal */
                pl_remove(&pl, pid_fils);
            }
        }
    } while (pid_fils > 0);
    /* autres actions après le suivi des changements d'état */
}

void fwd_sig_stop(int sig)
{
    if (!in_prompt)
    {
        // printf("fwd_sig_stop %d\n", pid_fils);
        kill(pid_fils, SIGSTOP);
    }
}

void fwd_sig_kill(int sig)
{
    if (!in_prompt)
    {
        // printf("fwd_sig_kill %d\n", pid_fils);
        kill(pid_fils, SIGKILL);
    }
}

int main(int argc, char const *argv[])
{
    struct sigaction handler_sigchld, handler_fwd_stop, handler_fwd_kill, handler_mask;
    handler_sigchld.sa_handler = suivi_fils;
    handler_fwd_stop.sa_handler = fwd_sig_stop;
    handler_fwd_kill.sa_handler = fwd_sig_kill;
    handler_mask.sa_handler = SIG_IGN;

    sigaction(SIGCHLD, &handler_sigchld, NULL);
    sigaction(SIGTSTP, &handler_fwd_stop, NULL);
    sigaction(SIGINT, &handler_fwd_kill, NULL);

    struct cmdline *cmdl;
    char cwd[1024];
    int id;
    while (1)
    {
        getcwd(cwd, sizeof(cwd));
        printf("%s$ ", cwd);
        in_prompt = 1;
        do
        {
            cmdl = readcmd();
        } while (!cmdl || !cmdl->seq[0]);
        in_prompt = 0;

        if (!builtin(&pl, cmdl->seq[0]))
        {
            pid_fils = fork();
            if (pid_fils == -1)
            {
                perror("fork");
                break;
            }
            if (pid_fils == 0)
            {
                sigaction(SIGTSTP, &handler_mask, NULL);
                sigaction(SIGINT, &handler_mask, NULL);
                execvp(cmdl->seq[0][0], cmdl->seq[0]);
                perror(cmdl->seq[0][0]);
                exit(getpid());
            }
            else
            {
                id = pl_add(&pl, pid_fils, cmdl->seq[0]);
                if (cmdl->backgrounded)
                {
                    printf("[%d] %d\n", id, pid_fils);
                }
                else
                {
                    pause();
                }
            }
        }
    }

    return 1; // Pas normal si on se retrouve ici...
}
