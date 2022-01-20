#include "include/elf_structure.h"

void GetElfFilename(const char *argv0_probe, char* deviceName, char* fullPath, char* elfFilename)
{
    int i;

    int lenght = strlen(argv0_probe); 
    int twopointsindex  = 0; 
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
	twopointsindex = i; 
	break; 
      }
   }

    // set deviceName to device name (ex: 'mass:', 'host:', 'mc0', etc) 
    strncpy(deviceName, argv0_probe, twopointsindex+1); 
    deviceName[twopointsindex+1] = 0;
    
    // set fullPath to full path including '/' (ex: 'mass:directory/', 'mass:directory1/directory2/', etc (no limit over depth)) 
    if (isFileOnRoot) 
    {
      strcpy(fullPath, deviceName); // fullPath = deviceName actually else
    }
    
    else 
    { strncpy(fullPath, argv0_probe, slashIndex+1); fullPath[slashIndex+1] = 0; 
    }

    // set elfFilename
    if (isFileOnRoot)
    {
        memcpy(elfFilename, argv0_probe + twopointsindex + 1, lenght - twopointsindex);
    }
     else
     {
        memcpy(elfFilename, argv0_probe + slashIndex + 1, lenght - slashIndex);
     }
}   

int getDisplayModeFromELFName(char **argv)
{ 
   char tmp[10]; 
   int ln = 0,i,j; 
   ln = strlen(argv[0]); 
   if (ln>=8) 
   { 
     // check for PAL 
     j=0; 
     for(i=ln-7; i<=ln-5; i++) 
     tmp[j++] = argv[0][i]; 
     tmp[j] = 0;

     if (strcmp(tmp, "PAL") == 0 || strcmp(tmp, "pal") == 0) 
    return 0; 
    
    // check for NTSC 
    j=0; 
    for(i=ln-8; i<=ln-5; i++) 
	tmp[j++] = argv[0][i]; 
    tmp[j] = 0;
	
     if (strcmp(tmp, "NTSC") == 0 || strcmp(tmp, "ntsc") == 0)
            return 1;
    }
   
    return -1;

}
													    
