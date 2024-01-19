#include <stdint.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[])
{
	printf("glibc: Hello, World!\n");
	fflush(stdout);
	return 0;
}
