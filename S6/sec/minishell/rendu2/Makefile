all: minishell q1 q3 q4 q5 q6 q7 q8 q9q10

minishell: minishell.c readcmd.c builtin.c process.c
	gcc -o $@ $^ -Wall -Werror

q1: q1.c readcmd.c
	gcc -o $@ $^ -Wall -Werror

q3: q3.c readcmd.c
	gcc -o $@ $^ -Wall -Werror

q4: q4.c readcmd.c builtin.c
	gcc -o $@ $^ -Wall -Werror

q5: q5.c readcmd.c builtin.c
	gcc -o $@ $^ -Wall -Werror

q6: q6.c readcmd.c builtin.c process.c
	gcc -o $@ $^ -Wall -Werror

q7: q7.c readcmd.c builtin.c process.c
	gcc -o $@ $^ -Wall -Werror

q8: q8.c readcmd.c builtin.c process.c
	gcc -o $@ $^ -Wall -Werror

q9q10: q9q10.c readcmd.c builtin.c process.c
	gcc -o $@ $^ -Wall -Werror

clean:
	rm -f minishell q1 q3 q4 q5 q6 q7 q8 q9q10
