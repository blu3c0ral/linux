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

int
main(int argc, char **argv) 
{
    char *file_ptr;

    file_ptr = get_file_ptr(argv[1]);

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