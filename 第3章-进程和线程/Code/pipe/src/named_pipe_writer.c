#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


const char pipe_path[] = "/home/molesir/Fifos/testfifo";


int main(void)
{
	// create named pipe
	int ret = mkfifo(pipe_path , 0664);
	if (ret == -1)
	{
		perror("mkfifo");
		exit(0);
	}
	printf("create fifo pipe successly\n");

	// open pipe
	int wfd = open(pipe_path, O_WRONLY);
	if (wfd == -1)
	{
		perror("open");
		exit(0);
	}
	printf("open fifo pipe by writing");

	// write pipe
	int i = 0;
	while (i < 100)
	{
		char buf[1024];
		sprintf(buf, "hello, fifo pipe, I am writing pipe...%d\n", i);
		write(wfd, buf, strlen(buf));
		++i;
		sleep(1);
	}
	close(wfd);

	return 0;
}
