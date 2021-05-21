// Question 4 (Commandes internes)

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "readcmd.h"
#include "builtin.h"

#define GREEN "\x1B[32m" // pour que le prompt soit joli
#define RESET "\x1B[0m"  // reset la couleur

// prompt
static void print_prompt()
{
    printf(GREEN "%s" RESET "$ ", getcwd(NULL, 0));
}

int main(int argc, char const *argv[])
{
    // variables locales
    struct cmdline *cmdl;
    pid_t pid_fils;

    while (1)
    {
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
        if (!builtin(NULL, cmdl->seq[0], NULL))
        {
            pid_fils = fork();
            if (pid_fils == -1)
            {
                perror("fork");
                continue;
            }
            if (!pid_fils) // fils
            {
                // exec
                execvp(cmdl->seq[0][0], cmdl->seq[0]);
                // si fail
                perror(cmdl->seq[0][0]);
                exit(getpid());
            }
            else // pere
            {
                pause();
            }
        }
    }

    return 1; // Pas normal si on se retrouve ici...
}
