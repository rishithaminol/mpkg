#include <stdio.h>
#include <stdlib.h>

#define PRINTF_I(X) printf("%s = %d\n", #X, X);
#define DO_STATEMENT(X) \
	X;	\
	printf("%s\n", #X)

int main(int argc, char *argv[])
{
	int a;
	int *p;
	char **s = (char **)malloc(sizeof(char *) * 2);
	s = (char *[]){argv[1], NULL};

	printf("%s\n", s[0]);

	/*PRINTF_I(sizeof(char **));

	p = (int *)malloc(4 * sizeof(int));
	*p = 3;
	*(p + 1) = 2;
	*(p + 2) = 45;
	*(p + 3) =  54;*/
	return 0;
}
