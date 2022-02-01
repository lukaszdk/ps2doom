#include <mixer/wav.h>

// Note: This calls fseek, so watch out...
int GetFilesize(FILE * fd)
{
	int size;
	fseek(fd, 0, SEEK_END);
	size = (int)ftell(fd);
	fseek(fd, 0, SEEK_SET);
	return (size);
}


int GetWAVsize(char *filename)
{
    int size;
    FILE *wav;
    wav = fopen(filename, "rb");
    
    if (wav != NULL) 
    {
	fseek(wav, 0, SEEK_END); 
	size = (int)ftell(wav); 
	size -= 0x30; fclose(wav); 
	return size;
    } else 
	return 0;

}

