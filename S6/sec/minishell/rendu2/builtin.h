#ifndef BUILTIN
#define BUILTIN

#include "process.h"

int builtin(struct process **pl, char **cmd, pid_t *pid_fg);

#endif
