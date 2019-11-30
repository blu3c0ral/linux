#include <stdio.h>

#include "elfutils.h"
#include "elf64parser.h"

#include "../memory.h"
#include "../fileutils.h"

#include <elf.h>

int _main(int argc, char **argv) 
{
    File_Ptr *f_ptr;

    f_ptr = f_open(argv[1]);

    switch (get_elf_class(f_ptr))
    {
        case ELFCLASS32:
            break;
        
        case ELFCLASS64:
            parse_elf64(f_ptr);
            break;
        
        default:
            break;
    }

    return 0;
}

int __main(int argc, char **argv)
{
    Elf64_DynExArr *dynexarr;
    size_t i;

    MALLOC(dynexarr, 1);
    MALLOC(dynexarr->elf64_dynex,2);
    dynexarr->size = 2;
    dynexarr->elf64_dynex[0].elf64_dyn.d_tag = DT_NEEDED;
    dynexarr->elf64_dynex[0].elf64_dyn.d_un.d_val = 4;

    dynexarr->elf64_dynex[1].elf64_dyn.d_tag = DT_STRTAB;
    dynexarr->elf64_dynex[1].elf64_dyn.d_un.d_ptr = 5;

    for(i = 0; i < dynexarr->size; ++i)
    {
        printf("%ld. d_tag is: %ld, d_un is: %ld\n", i, dynexarr->elf64_dynex[i].elf64_dyn.d_tag, dynexarr->elf64_dynex[i].elf64_dyn.d_un.d_ptr);
    }
}

int main(int argc, char **argv)
{
    __main(argc, argv);
}