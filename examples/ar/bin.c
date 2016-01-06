#include <stdio.h>

void bin(unsigned n)
{
    unsigned i;
    for (i = 1 << 31; i > 0; i = i / 2)
        (n & i)? printf("1"): printf("0");
}

int main(void)
{
	int x = 0756;
	printf("%d\n", x);
    bin(-5);
    printf("\n%x\n", -5);
    bin(5);
    printf("\n%x\n", 5);
}
