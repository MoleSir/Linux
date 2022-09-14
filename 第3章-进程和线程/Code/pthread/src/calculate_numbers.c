// use thread to get some calulation
#include <stdio.h>
#include <pthread.h>


int fibonacci(unsigned int n)
{
	// base case
	if (n == 0 || n == 1 || n == 2) return n;
	// recursive call
	return fibonacci(n - 1) + fibonacci(n - 2);
}

int factorial(unsigned int n)
{
	// base case
	if (n == 0 || n == 1) return 1;
	// recursive call
	else return n * factorial(n - 1);
}

int power2(unsigned int order)
{
	unsigned int i;
	int result = 1;
	for (i = 0; i < order; ++i)
		result *= 2;
	return result;
}


// arg struct
typedef int (* calculate_func_t)(unsigned int);
typedef struct
{
	calculate_func_t handler;
	int arg;
	int result;
} arg_t ;

// pthread function
void* calculate(void* arg)
{
	// transfor
	arg_t* parg = (arg_t*)arg;
	// calculate
	parg->result = parg->handler(parg->arg);
	printf("child thread %ld finish calculation\n", pthread_self());
	pthread_exit(NULL);
}


int main()
{
	// three arg_t
	arg_t args[3] = { { &fibonacci, 12, 0 },
       			  { &factorial, 10, 0 },
			  { &power2, 15, 0 } };
	// create threads
	pthread_t tids[3];
	int i;
	for (i = 0; i < 3; ++i)
	{
		pthread_create(tids + i, NULL, calculate, args + i);
		printf("create child thread %ld\n", tids[i]);
	}
	
	// join
	for (i = 0; i < 3; ++i)
		pthread_join(tids[i], NULL);

	// print result
	printf("finish calcalate!\n");
	printf("fibonacci[%d] = %d\n", args[0].arg, args[0].result);
	printf("!%d = %d\n", args[1].arg, args[1].result);
	printf("2^%d = %d\n", args[2].arg, args[2].result);

	return 0;
}


