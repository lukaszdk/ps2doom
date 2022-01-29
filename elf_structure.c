#include "include/elf_structure.h"
#include <elf.h>
#include <stdlib.h>
#include <libcdvd-common.h>

typedef struct
 {
     u8 ident[16];  // struct definition for ELF object header
     u16 type;
     u16 machine;
     u32 version;
     u32 entry;
     u32 phoff;
     u32 shoff;
     u32 flags;
     u16 ehsize;
     u16 phentsize;
     u16 phnum;
     u16 shentsize;
     u16 shnum;
     u16 shstrndx;
 } elf_header_t;
  
 typedef struct
 {
     u32 type;  // struct definition for ELF program section header
     u32 offset;
     void *vaddr;
     u32 paddr;
     u32 filesz;
     u32 memsz;
     u32 flags;
     u32 align;
 } elf_pheader_t;
  

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
					


void elf_loader()
{
    char args;
    int i;
    char boot_elf;
    char CNF_RAM_p;
    char CDVD_INIT_EXIT;
       elf_header_t *boot_header;
    elf_pheader_t *boot_pheader; 
    // Load & execute embedded loader from here
    boot_elf = (u8 *)&elf_loader;

    // Get Elf header
    boot_header;
    

    // Check elf magic
    if (((u32*)boot_header->ident) != ELFMAG)
        return;

    // Get program headers
    boot_pheader = (elf_pheader_t *)(boot_elf + boot_header->phoff);

    // Scan through the ELF's program headers and copy them into apropriate RAM
    // section, then padd with zeros if needed.
    for (i = 0; i < boot_header->phnum; i++) {

        if (boot_pheader[i].type != PT_LOAD)
            continue;

        memcpy(boot_pheader[i].vaddr, boot_elf + boot_pheader[i].offset, boot_pheader[i].filesz);

        if (boot_pheader[i].memsz > boot_pheader[i].filesz)
            memset(boot_pheader[i].vaddr + boot_pheader[i].filesz, 0, boot_pheader[i].memsz - boot_pheader[i].filesz);
    }

    if (CNF_RAM_p != NULL)
        free(CNF_RAM_p);
    sceCdInit(CDVD_INIT_EXIT);
       SifExitRpc();

    // Execute Elf Loader
    ExecPS2((void *)boot_header->entry, 0, 1, args);

}

