#include "utils.h"
#include "elfutils.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

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

char *get_file_ptr(const char *fpath)
{
    int fd;
    int OFLAGS = O_RDONLY;
    char *file_ptr;
    long fsize;

    fd  = open(fpath, OFLAGS);

    fsize = file_size(fd);
    if (fsize == -1) 
    {
        error_exit("ERROR: file size couldn't retrieved");
    }
    else if (fsize < sizeof(Elf64_Ehdr))
    {
        error_exit("ERROR: file is not an ELF file");
    }

    file_ptr = (char *)mmap(NULL, fsize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_ptr == NULL) 
    {
        error_exit("ERROR: file couldn't be mapped to memory");
    }

    return file_ptr;
}
