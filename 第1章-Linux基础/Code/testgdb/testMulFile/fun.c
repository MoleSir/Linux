#include "fun.h"

void print_triangle(int n)
{
        // print n lines
        for (int i = 0; i < n; i++)
        {
                // print space
                for (int j = 0; j < n - i; j++)
                {
                        putchar(' ');
                }
                // print *
                for (int j = 0; j < 2 * i + 1; j++)
                {
                        putchar('*');
		}
		// print '\n'
		putchar('\n');
        }
}
