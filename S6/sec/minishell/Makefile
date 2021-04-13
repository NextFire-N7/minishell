all: minishell

minishell: readcmd.c process_list.c minishell.c
	gcc -o $@ $^

clean:
	rm -f minishell
