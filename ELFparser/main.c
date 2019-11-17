#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>

#include <elf.h>

#include "elfutils.h"
#include "elf64parser.h"


void error_exit(const char *errmsg) {
    fprintf(stderr, "%s\n", errmsg);
    exit(1);
}

long
file_size(int fd)
{
    __off_t size = 0;
    size = lseek(fd, 0, SEEK_END);
    if (size == -1) {
        return -1;
    }
    lseek(fd, 0, SEEK_SET);
    return (long)size;
}

int
main(int argc, char **argv) 
{
    int fd;
    int OFLAGS = O_RDONLY;
    char *file_ptr;
    long fsize;

    if (argc < 2) 
    {
        printf("Please provide an ELF file\n");
        return 0;
    }

    fd  = open(argv[1], OFLAGS);

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

    switch (get_elf_class(file_ptr))
    {
    case ELFCLASS32:
        break;
    
    case ELFCLASS64:
        parse_elf64(file_ptr);
        break;
    
    default:
        break;
    }

    return 0;
}