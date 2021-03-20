#include "cosmitoFileIO.h"

// Note: This calls fseek, so watch out...
int GetFilesize(FILE * fd)
{
	int size;
	fseek(fd, 0, SEEK_END);
	size = (int)ftell(fd);
	fseek(fd, 0, SEEK_SET);
	return (size);
}

///
int GetWAVsize(char *filename)
{
    int size;
    FILE *wav;
    wav = fopen(filename, "rb");

    if (wav != NULL)
    {
        fseek(wav, 0, SEEK_END);
        size = (int)ftell(wav);
        size -= 0x30;
        fclose(wav);
        return size;
    }
    else
        return 0;
}

/// TBD
//// Dynamically mallocs a signed char buffer and loads from file into it. res < 0 means error.
//FILE AllocLoad_scharbuffer(char *filename, int optionalBufSize, int *res)
//{
//	FILE *fd;
//	int size;
//
//
//	if (optionalBufSize == 0)
//
//}

///
void GetElfFilename(const char *argv0_probe, char* deviceName, char* fullPath, char* elfFilename)
{
    int i;

    int lenght = strlen(argv0_probe);
    int doispontosIndex = 0;
    int slashIndex = 0;
    int isFileOnRoot = 0;

    
    // locate '/' from the end of path
    for(i=lenght-1; i>=0; i--)
    {
        if (argv0_probe[i] == '/')
        {
            slashIndex = i;
            break;
        }
    }
    if (slashIndex == 0)
        isFileOnRoot = 1;       // elf is located on root of device

    // locate ':' from the start of path
    for(i=0; i<lenght; i++)
    {
        if (argv0_probe[i] == ':')
        {
            doispontosIndex = i;
            break;
        }
    }

    // set deviceName to device name (ex: 'mass:', 'host:', 'mc0', etc)
    strncpy(deviceName, argv0_probe, doispontosIndex+1);
    deviceName[doispontosIndex+1] = 0;

    // set fullPath to full path including '/' (ex: 'mass:directory/', 'mass:directory1/directory2/', etc (no limit over depth))
    if (isFileOnRoot)
        strcpy(fullPath, deviceName);   // fullPath = deviceName actually
    else
    {
        strncpy(fullPath, argv0_probe, slashIndex+1);   
        fullPath[slashIndex+1] = 0;
    }

    // set elfFilename
    if (isFileOnRoot)
        memcpy(elfFilename, argv0_probe + doispontosIndex + 1, lenght - doispontosIndex);
    else
        memcpy(elfFilename, argv0_probe + slashIndex + 1, lenght - slashIndex);
}
