all: minishell

minishell: readcmd.c builtin.c process.c Q6.c
	gcc -o $@ $^

clean:
	rm -f minishell
