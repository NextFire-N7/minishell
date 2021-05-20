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

#define MAX_PIPES 10
#define GREEN "\x1B[32m"
#define RESET "\x1B[0m"

struct process *pl = NULL;
pid_t pid_fils;
int in_prompt;
int fd_stdin, fd_stdout;

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

int redirections(struct cmdline cmdl)
{
    if (cmdl.in)
    {
        int fd_in = open(cmdl.in, O_RDONLY);
        if (fd_in == -1)
        {
            return -1;
        }
        dup2(fd_in, STDIN_FILENO);
    }
    if (cmdl.out)
    {
        int fd_out = open(cmdl.out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_out == -1)
        {
            return -1;
        }
        dup2(fd_out, STDOUT_FILENO);
    }
    return 0;
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

    fd_stdin = dup(STDIN_FILENO);
    fd_stdout = dup(STDOUT_FILENO);

    struct cmdline *cmdl;
    char *cwd;
    int id;
    int i;
    int pipes[MAX_PIPES][2];

    while (1)
    {
        dup2(fd_stdin, STDIN_FILENO);
        dup2(fd_stdin, STDOUT_FILENO);

        cwd = getcwd(NULL, 0);
        printf(GREEN "%s" RESET "$ ", cwd);
        in_prompt = 1;
        do
        {
            cmdl = readcmd();
        } while (!cmdl || !cmdl->seq);
        in_prompt = 0;

        if (redirections(*cmdl) == -1)
        {
            perror("redirections");
            continue;
        }

        i = -1;
        while (cmdl->seq[++i])
        {
            if (cmdl->seq[i + 1])
            {
                if (pipe(pipes[i]) == -1)
                {
                    perror("pipe");
                }
            }
            if (i > 0)
            {
                close(pipes[i - 1][1]);
            }
            if (!builtin(&pl, cmdl->seq[i]))
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
                    if (i > 0)
                    {
                        dup2(pipes[i - 1][0], STDIN_FILENO);
                    }
                    if (cmdl->seq[i + 1])
                    {
                        dup2(pipes[i][1], STDOUT_FILENO);
                    }
                    execvp(cmdl->seq[i][0], cmdl->seq[i]);
                    perror(cmdl->seq[i][0]);
                    exit(getpid());
                }
                else
                {
                    if (i > 0)
                    {
                        close(pipes[i - 1][0]);
                    }
                    id = pl_add(&pl, pid_fils, cmdl->seq[i]);
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
    }

    return 1; // Pas normal si on se retrouve ici...
}
