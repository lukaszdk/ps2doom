#include <tamtypes.h>
#include <stdio.h>
#include <kernel.h>
#include <math.h>


int gethostname(char *name, int len)
{
	return -1;
}

u32 inet_addr(const char *cp)
{
	return 0;
}

extern double pow (double a, double b)
{
	return powf(a,b);
}

void setbuf (FILE *__restrict, char *__restrict)
{
	return 0;
}
