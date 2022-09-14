#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>


const char pipe_path[] = "/home/molesir/Fifos/testfifo";


int main(void)
{
	// open pipe
	int rfd = open(pipe_path, O_RDONLY);
	if (rfd == -1)
	{
		perror("open");
		exit(0);
	}
	printf("open fifo pipe by reading..\n");

	// read
	while (1)
	{
		char buf[1024];
		memset(buf, 0, sizeof(buf));
		
		int len = read(rfd, buf, sizeof(buf));
		printf("the data from pipe: %s\n", buf);
		if (len == 0)
		{
			// close write port
			printf("the write port already closed\n");
			break;
		}
	}

	close(rfd);

	return 0;
}
