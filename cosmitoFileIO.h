#ifndef _COSMITOFILEIO
#define _COSMITOFILEIO

#include <stdio.h>
#include <string.h>

// Note: This calls fseek, so watch out...
int GetFilesize(FILE * fd);

int GetWAVsize(char *filename);

void GetElfFilename(const char *argv0_probe, char* deviceName, char* fullPath, char* elfFilename);

#endif // _COSMITOFILEIO
