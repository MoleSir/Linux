#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>


int main(void)
{
	// create pipe
	int fd[2];
	int ret = pipe(fd);
	if (ret == -1)
	{
		perror("pipe");
		return -1;
	}

	// creta child porcess
	pid_t pid;
	if ( !(pid = fork()) )
	{
		// child process: close read port, write only
		close(fd[0]);
		// redirct port
		dup2(fd[1], STDOUT_FILENO);
		// call execlp
		execlp("ps", "ps", "aux", NULL);
	}

	// father process: close write port, only read
	close(fd[1]);
	char buf[1024] = {0};
	while (1)
	{
		memset(buf, 0, sizeof(buf));
		int len = read(fd[0], buf, sizeof(buf));
		// read return 0 --> finish read
		if (len == 0) break;

		printf("%s, len = %d\n", buf, len);
	}

	// close read
	close(fd[0]);

	// wait child
	wait(NULL);

	return 0;
}
