#include <stdio.h>
#include "readcmd.h"

int main(int argc, char const *argv[])
{
    struct cmdline *cmd;
    while (1)
    {
        printf(">>> ");
        cmd = readcmd();
    }
}
