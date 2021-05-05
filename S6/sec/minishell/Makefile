all: minishell

minishell: readcmd.c builtin.c process.c minishell.c
	gcc -o $@ $^ -Wall -Werror

clean:
	rm -f minishell
