all: minishell Q1 Q3 Q4 Q5 Q6 Q7 Q8 Q10

minishell: minishell.c readcmd.c builtin.c process.c
	gcc -o $@ $^ -Wall -Werror

Q1: Q1.c readcmd.c
	gcc -o $@ $^ -Wall -Werror

Q3: Q3.c readcmd.c
	gcc -o $@ $^ -Wall -Werror

Q4: Q4.c readcmd.c builtin.c process.c
	gcc -o $@ $^ -Wall -Werror

Q5: Q5.c readcmd.c builtin.c process.c
	gcc -o $@ $^ -Wall -Werror

Q6: Q6.c readcmd.c builtin.c process.c
	gcc -o $@ $^ -Wall -Werror

Q7: Q7.c readcmd.c builtin.c process.c
	gcc -o $@ $^ -Wall -Werror

Q8: Q8.c readcmd.c builtin.c process.c
	gcc -o $@ $^ -Wall -Werror

Q10: Q10.c readcmd.c builtin.c process.c
	gcc -o $@ $^ -Wall -Werror

clean:
	rm -f minishell Q1 Q3 Q4 Q5 Q6 Q7 Q8 Q10
