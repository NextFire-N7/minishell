#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include "readcmd.h"
#include "processlist.h"

struct process_list pl;

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

void jobs(char **cmd) {}

void stop(char **cmd) {}

void bg(char **cmd) {}

void fg(char **cmd) {}

int builtin(char **cmd)
{
    int is_builtin = 1;
    if (!strcmp(cmd[0], "cd"))
    {
        chdir(cmd[1]);
    }
    else if (!strcmp(cmd[0], "exit"))
    {
        exit(EXIT_SUCCESS);
    }
    else if (!strcmp(cmd[0], "jobs"))
    {
        jobs(cmd);
    }
    else if (!strcmp(cmd[0], "stop"))
    {
        stop(cmd);
    }
    else if (!strcmp(cmd[0], "bg"))
    {
        bg(cmd);
    }
    else if (!strcmp(cmd[0], "fg"))
    {
        fg(cmd);
    }
    else
    {
        is_builtin = 0;
    }
    return is_builtin;
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
        if (!builtin(cmd->seq[0]))
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
                pl_add(&pl, pid_fils, cmd->seq[0]);
                if (!cmd->backgrounded)
                {
                    wait(NULL);
                }
                break;
            }
        }
    }

    return EXIT_FAILURE; // Pas normal si on se retrouve ici...
}
