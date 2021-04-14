all: minishell

minishell: readcmd.c processlist.c minishell.c
	gcc -o $@ $^

clean:
	rm -f minishell
