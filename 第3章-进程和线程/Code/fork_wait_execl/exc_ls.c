#include <stdio.h>
#include <unistd.h>

int main(void)
{
	pid_t pid;

	if (!(pid = fork()))
	{
		execlp("ps", "title", "aux", NULL);
		printf("execlp ps failed\n");
	}
	else
	{
		printf("father process...\n");
	}
	
	wait(NULL);
	printf("process %ld finish\n", getpid());

	return 0;
}
