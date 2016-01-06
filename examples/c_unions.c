#include <stdio.h>

typedef enum {INTS, FLOATS, DOUBLE} Type;
typedef struct {
	Type s_type;
	union {
		int s_ints;
		float s_floats;
		double s_double;
	};
} S;

union foo {int i; double d;};

int c_unions1(void)
{
	int x;
	double y;
	union foo u;

	x = 32;
	y = 234.23434;

	u = (union foo)x; // == (u.i = x)
	u = (union foo)y; // == (u.d = y)

	printf("%d\n", u.d);

	return 0;
}

int main()
{
	c_unions1();

	return 0;
}