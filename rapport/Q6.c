// Question 6 (Gerer les processus lances depuis le shell)

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include "readcmd.h"
#include "builtin.h"
#include "process.h"

#define GREEN "\x1B[32m" // pour que le prompt soit joli
#define RESET "\x1B[0m"  // reset la couleur

static struct process *pl = NULL; // liste chainee des processus
static pid_t pid_fg;              // pid du processus en fg

static void suivi_fils(int sig)
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
            if (pid_fg && pid_fils != pid_fg)
            {
                /* reprise attente processus avant plan */
                pause();
            }
        }
    } while (pid_fils > 0);
    /* autres actions après le suivi des changements d'état */
}

// prompt
static void print_prompt()
{
    printf(GREEN "%s" RESET "$ ", getcwd(NULL, 0));
}

// SIGSTP -> SIGSTOP
static void fwd_sig_stop(int sig)
{
    printf("\n");
    if (pid_fg)
    {
        kill(pid_fg, SIGSTOP);
    }
    else
    {
        print_prompt();
    }
}

int main(int argc, char const *argv[])
{
    // def des handlers
    struct sigaction handler_sigchld, handler_fwd_stop, handler_mask;
    handler_sigchld.sa_handler = suivi_fils;
    handler_fwd_stop.sa_handler = fwd_sig_stop;
    handler_mask.sa_handler = SIG_IGN;

    // attribution des handlers
    sigaction(SIGCHLD, &handler_sigchld, NULL);
    sigaction(SIGTSTP, &handler_fwd_stop, NULL);

    // variables locales
    struct cmdline *cmdl;
    pid_t pid_fils;
    int id;

    while (1)
    {
        pid_fg = 0;     // pas de processus en avant-plan
        print_prompt(); // affichage prompt
        do
        {
            cmdl = readcmd(); // lecture de la ligne de cmd
        } while (!cmdl);

        // affichage erreurs cmdl
        if (cmdl->err)
        {
            fprintf(stderr, "%s\n", cmdl->err);
            continue;
        }

        // fork si pas une commande built-in
        if (!builtin(&pl, cmdl->seq[0], &pid_fg))
        {
            pid_fils = fork();
            if (pid_fils == -1)
            {
                perror("fork");
                continue;
            }
            if (!pid_fils) // fils
            {
                // masquage SIGTSTP
                sigaction(SIGTSTP, &handler_mask, NULL);
                // exec
                execvp(cmdl->seq[0][0], cmdl->seq[0]);
                // si fail
                perror(cmdl->seq[0][0]);
                exit(getpid());
            }
            else // pere
            {
                // ajout du process dans la liste
                id = pl_add(&pl, pid_fils, cmdl->seq[0]);
                // gestion bg ou pas
                if (cmdl->backgrounded)
                {
                    printf("[%d] %d\n", id, pid_fils);
                }
                else
                {
                    pid_fg = pid_fils;
                    pause();
                }
            }
        }
    }

    return 1; // Pas normal si on se retrouve ici...
}
