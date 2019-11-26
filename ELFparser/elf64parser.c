#include "elf64parser.h"

#include <stdint.h>

#include <elf.h>

#include "elfutils.h"
#include "memory.h"

uint8_t 
parse_elf64(char *fptr) 
{
    Elf64_Ehdr * elf64_ehdr;

    elf64_ehdr = get_Ehdr64(fptr);
}



Elf64_Shdr *get_elf64_shdr(char *file_ptr, Elf64_Ehdr *elf64_ehdr)
{
    Elf64_Shdr * shdrs_arr;
    char *shdrs;
    Elf64_Off offset; 
    uint64_t i;

    MALLOC(shdrs_arr, elf64_ehdr->e_shnum);
    for(i=0; i<elf64_ehdr->e_shnum; ++i)
    {
        offset = i * elf64_ehdr->e_shentsize;
        memcpy((char *)shdrs_arr + offset, file_ptr + elf64_ehdr->e_shoff + offset, elf64_ehdr->e_shentsize);
    }

    return shdrs_arr;
}



Elf64_DynExArr *ldd_data(char *f_path)
{
    char *fptr;
    Elf64_Ehdr *elf64_ehdr;
    Elf64_Shdr *elf64_shdr;
    Elf64_Off strtbl;
    Elf64_Off dynsec;
    Elf64_Dyn *elf64_dyn;
    uint64_t i;
    size_t dyncount;
    size_t dyncap;


    fptr = get_file_ptr(f_path);
    elf64_ehdr = get_Ehdr64(fptr);
    elf64_shdr = get_elf64_shdr(fptr, elf64_ehdr);

    /* Find string table and dynamic section */
    for(i = 0; i < elf64_ehdr->e_shnum; ++i)
    {
        if (elf64_shdr[i].sh_type == SHT_DYNAMIC)
        {
            dynsec = elf64_shdr[i].sh_offset;
            break;
        }
    }

    /* Continue from here */
    MALLOC(elf64_dyn, 1);
    dyncap = 1;
    do
    {
        ++dyncount;
        if(dyncount == dyncap)
        {
            dyncap *= 2;
            REALLOC(elf64_dyn, dyncap);
        }
    } while (elf64_dyn[dyncount - 1].d_tag != DT_NULL);
    
}