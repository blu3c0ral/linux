#include <elf.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

#include "elfutils.h"

#include "../memory.h"
#include "../fileutils.h"
#include "../utils.h"



int8_t get_elf_class(File_Ptr *f_ptr) 
{
    char *elfmag;
    size_t b_read;
    size_t to_read;
    int8_t ret_val;

    to_read = EI_CLASS + 1;

    MALLOC(elfmag, to_read);
    b_read = f_read(elfmag, f_ptr, 0, to_read);

    if (b_read < to_read)
    {
        return -1;
    }

    for (size_t i = 0; i < 4; ++i)
    {
        if (elfmag[i] != ELFMAG[i])
        {
            return -1;
        }
    }

    ret_val = elfmag[to_read - 1];
    FREE(elfmag);
    return ret_val;
}


Elf64_Ehdr *get_Ehdr64(File_Ptr *f_ptr)
{
    Elf64_Ehdr *elf64_ehdr;
    size_t b_read;
    size_t eh_size;

    eh_size = sizeof(Elf64_Ehdr);
    MALLOC(elf64_ehdr, 1);
    b_read = f_read(elf64_ehdr, f_ptr, 0, eh_size);

    if (b_read < eh_size)
    {
        error_msg("error - can't get Ehdr64 from file");
        FREE(elf64_ehdr);
        return NULL;
    }

    return elf64_ehdr;
}


Elf32_Ehdr *get_Ehdr32(File_Ptr *f_ptr)
{
    Elf32_Ehdr *elf32_ehdr;
    size_t b_read;
    size_t eh_size;

    eh_size = sizeof(Elf32_Ehdr);
    MALLOC(elf32_ehdr, 1);
    b_read = f_read(elf32_ehdr, f_ptr, 0, eh_size);

    if (b_read < eh_size)
    {
        error_msg("error - can't get Ehdr32 from file");
        FREE(elf32_ehdr);
        return NULL;
    }

    return elf32_ehdr;
}
