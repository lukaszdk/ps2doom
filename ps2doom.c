#include <tamtypes.h>
#include <stdio.h>
#include <kernel.h>
#include <math.h>
#include "ps2doom.h"

extern double pow (double a, double b)
{
	return powf(a,b);
}

void setbuf (FILE *__restrict, char *__restrict)
{
	return 0;
}


