#include "elfutils.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <elf.h>

int8_t 
get_elf_class(char *file_ptr) 
{
    for (size_t i = 0; i < 4; ++i)
    {
        if (file_ptr[i] != ELFMAG[i])
        {
            return -1;
        }
    }
    return file_ptr[EI_CLASS];
}

Elf64_Ehdr *
get_Ehdr64(char *file_ptr) 
{
    return (Elf64_Ehdr *)file_ptr;
}

Elf32_Ehdr * 
get_Ehdr32(char *file_ptr) 
{
    return (Elf32_Ehdr *)file_ptr;
}
