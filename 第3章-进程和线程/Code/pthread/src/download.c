#include <stdio.h>
#include <pthread.h>


void* download_file(void* arg)
{
	int seconds = *(int*)arg;
	pthread_t tid = pthread_self();

	printf("thread %ld download file...\n", tid);
	sleep(seconds);
	printf("thread %ld finish download!\n", tid);
}


int main(void)
{
	int seconds[3] = {5, 4, 3};
	pthread_t tids[3];
	int i;

	for (i = 0; i < 3; ++i)
	{
		pthread_create(tids + i, NULL, download_file, seconds + i);
		printf("cretae chlid thread %ld\n", tids[i]);
	}

	for (i = 0; i < 3; ++i)
		pthread_join(tids[i], NULL);

	printf("donwload all files\n");

	return 0;
}
