#include <stdio.h>
#include <unistd.h>

void download_file(int seconds)
{
	pid_t pid = getpid();
	printf("child process %ld downloadling file...\n", pid);
	sleep(seconds);
	printf("child process %ld finish download\n", pid);
}

int main()
{
	pid_t pids[3];
	int i;
	
	for (i = 0; i < 3; ++i)
	{
		if (!(pids[i] = fork()))
		{
			download_file(5 - i);
			exit(0);
		}
		else
			printf("create process %ld\n", pids[i]);
	}

	for (i = 0; i < 3; ++i)
		waitpid(pids[i], NULL, 0);
	
	printf("download all files!\n");

	return 0;
}
