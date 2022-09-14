#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
	pid_t pid1, pid2;
	if (!(pid1 = fork())) { while (1) printf("A"); }
	if (!(pid2 = fork())) { while (1) printf("B"); }

	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);

	return 0;
}
