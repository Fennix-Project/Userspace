#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include "aux.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

void test_args(int argc, char *argv[], char *envp[])
{
	printf("%p %p %p\n", (void *)(uint64_t)&argc, (void *)&argv, (void *)&envp);
	printf("I have %d arguments\n", argc);
	for (int i = 0; i < argc; i++)
		printf("argv[%d] = (%p) %s\n", i, argv[i], argv[i]);

	int envc = 0;
	while (envp[envc] != NULL)
		envc++;

	printf("I have %d environment variables\n", envc);
	for (int i = 0; i < envc; i++)
		printf("envp[%d] = (%p) %s\n", i, envp[i], envp[i]);

	Elf64_auxv_t *auxv;
	char **e = envp;

	while (*e++ != NULL)
		;

	for (auxv = (Elf64_auxv_t *)e; auxv->a_type != AT_NULL; auxv++)
		printf("auxv: %ld %#lx\n", auxv->a_type, auxv->a_un.a_val);
}

int main(int argc, char *argv[], char *envp[])
{
	printf("Hello, World!\n");
	// while (1);
	// test_args(argc, argv, envp);
	FILE *test = fopen("/test.txt", "r");
	if (test == NULL)
	{
		printf("Failed to open file\n");
		return -0xF11e;
	}

	printf("Test.txt contents: ");
	char ch;
	while (1)
	{
		ch = fgetc(test);
		if (ch == EOF)
		{
			printf("\n");
			break;
		}
		putchar(ch);
	}
	fclose(test);

	pid_t pid = fork();

	if (pid == 0) // Child process
	{
		pid_t pid2 = fork();
		if (pid == 0) // Child process
		{
			char *shebang_args[] = {"/test.sh", NULL};
			execv(shebang_args[0], shebang_args);
		}

		printf("Creating shell process\n");
		char *args[] = {"/bin/echo", "Hello, World!", NULL};
		execv(args[0], args);
		exit(EXIT_FAILURE);
	}
	else if (pid > 0)
	{
		printf("Waiting for child process %d to exit\n", pid);
		int status;
		wait(&status);
		int exited = WIFEXITED(status);
		if (exited)
		{
			int exit_code = WEXITSTATUS(status);
			printf("Child process exited with code: %d\n", exit_code);
			return exit_code;
		}
		else
		{
			printf("Execution failed. (%d)\n", exited);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		printf("\eFF0000Failed to create the process.\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
