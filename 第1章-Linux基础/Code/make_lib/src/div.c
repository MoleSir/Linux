#include "cal.h"
#include <stdio.h>

double div(double a, double b)
{
    if (b == 0.0) 
    {
        printf("error!\n");
        return 0;
    }
    return a/b;
}