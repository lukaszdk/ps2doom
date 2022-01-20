
// Hwc code thanks for it

void elf_loader(){
// Load & execute embedded loader from here    
    boot_elf = (u8 *)&elf_loader;
    
    // Get Elf header
    boot_header = (elf_header_t )boot_elf;
    
    // Check elf magic
    if (((u32*)boot_header->ident) != ELF_MAGIC) 
        return;

    // Get program headers
    boot_pheader = (elf_pheader_t *)(boot_elf + boot_header->phoff);
    
    // Scan through the ELF's program headers and copy them into apropriate RAM
    // section, then padd with zeros if needed.
    for (i = 0; i < boot_header->phnum; i++) {
        
        if (boot_pheader[i].type != ELF_PT_LOAD)
            continue;

        memcpy(boot_pheader[i].vaddr, boot_elf + boot_pheader[i].offset, boot_pheader[i].filesz);
    
        if (boot_pheader[i].memsz > boot_pheader[i].filesz)
            memset(boot_pheader[i].vaddr + boot_pheader[i].filesz, 0, boot_pheader[i].memsz - boot_pheader[i].filesz);
    }        
    
    if (CNF_RAM_p != NULL)
        free(CNF_RAM_p);        
    cdInit(CDVD_INIT_EXIT);
       SifExitRpc();
    
    // Execute Elf Loader
    ExecPS2((void *)boot_header->entry, 0, 1, args);
 
} 
