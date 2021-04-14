all: minishell

minishell: readcmd.c builtin.c processlist.c minishell.c
	gcc -o $@ $^

clean:
	rm -f minishell
