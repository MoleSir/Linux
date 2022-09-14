#include "fun.h"
#include <string.h>

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		printf("please input the lines number of Triangle Tower!\n");
	}
	else
	{
		int n = atoi(argv[1]);
		print_triangle(n);
	}
	return 0;
}
