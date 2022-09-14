#include "cal.h"
#include <stdio.h>

int main()
{
    double r1 = add(1.2, 3.4);
    double r2 = div(3.6, 0.9);
    double r3 = mul(2.1, 33.3);
    double r4 = sub(212.2, 9.8);

    printf("r1 = %lf, r2 = %lf, r3 = %lf, r4 = %lf\n", r1, r2, r3, r4);

    return 0;
}