#include <floatlib.h>
#include <stdio.h>
#include <kernel.h>
int gethostname(char *name, int len)
{
	return -1;
}

u32 inet_addr(const char *cp)
{
	return 0;
}

float pow(float a, float b)
{
	return powf(a,b);
}

int setbuf(FILE * stream, char * buffer)
{
	return 0;
}
