#include <stdio.h>

void print(int x)
{
	printf("%d\n", x);
}

void hello(void (*extern_func)(int))
{
	extern_func(12);
}

int main()
{
	hello(&print);

	return 0;
}
