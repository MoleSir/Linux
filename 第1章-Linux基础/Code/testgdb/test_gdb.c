#include <stdio.h>

int fab(int n)
{
	// bad argument
	if (n == -1)
	{
		printf("error argument");
		return -1;
	}
	// base case
	if (n == 1 || n == 2)
		return 1;
	// tree recursion call
	return fab(n - 1) + fab(n - 2);
}

int fact(int n)
{
	// bad argument
	if (n < 0) 
	{
		printf("error argument!");
		return -1;
	}
	// base case
	if (n == 0)
	       return 1;
	// recursion call	
	return n * fact(n - 1);
}

int main(int argc, char* argv[])
{
	printf("%d arguments input\n", argc - 1);
	for (int i = 1; i < argc; ++i)
	{
		printf("No.%d argument: %s\n", i, argv[i]);
	}

	int fab_res = fab(5);
	int fact_res = fact(7);

	// print res
	printf("fab[5] = %d\n", fab_res);
	printf("7! = %d\n", fact_res);

	return 0;
}
