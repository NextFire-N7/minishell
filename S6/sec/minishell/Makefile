all: minishell

minishell: readcmd.c builtin.c process.c minishell.c
	gcc -o $@ $^

clean:
	rm -f minishell
