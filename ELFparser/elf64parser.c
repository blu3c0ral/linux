#include "elf64parser.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <elf.h>

#include "elfutils.h"
#include "../memory.h"
#include "../fileutils.h"
#include "../utils.h"

uint8_t parse_elf64(File_Ptr *fptr) 
{
    Elf64_Ehdr * elf64_ehdr;

    elf64_ehdr = get_Ehdr64(fptr);
}



Elf64_Shdr *get_elf64_shdr(File_Ptr *f_ptr, Elf64_Ehdr *elf64_ehdr)
{
    Elf64_Shdr * shdrs_arr;
    Elf64_Off offset;
    size_t b_read;
    uint64_t i;

    MALLOC(shdrs_arr, elf64_ehdr->e_shnum);
    for(i=0; i<elf64_ehdr->e_shnum; ++i)
    {
        offset = i * elf64_ehdr->e_shentsize;
        b_read = f_read((char *)shdrs_arr + offset, f_ptr, elf64_ehdr->e_shoff + offset, elf64_ehdr->e_shentsize);

        if (b_read < elf64_ehdr->e_shentsize)
        {
            error_msg("error - can't get the Shdr's");
            FREE(shdrs_arr);
            return NULL;
        }
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
    Elf64_DynExArr *elf64_dynexarr;
    uint64_t i;
    size_t dyn_size;
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


    /* Obtaining the dyn's structs */
    dyncap = 10;
    MALLOC(elf64_dyn, dyncap);
    dyn_size = sizeof(elf64_dyn);
    do
    {
        memcpy(elf64_dyn + dyncount, fptr + dynsec + dyncount * dyn_size, dyn_size);
        ++dyncount;
        if(dyncount == dyncap)
        {
            dyncap *= 2;
            REALLOC(elf64_dyn, dyncap);
        }
    } while (elf64_dyn[dyncount - 1].d_tag != DT_NULL);
    

    /* Find the string table */
    for (i = 0; i < dyncount; ++i)
    {
        if (elf64_dyn[i].d_tag == DT_STRTAB)
        {
            strtbl = (Elf64_Addr) elf64_dyn[i].d_un.d_ptr;      /* I'm not sure that Addr ?= Off */
            break;
        }
    }


    /* Allocate and fill the Elf64_DynExArr array */
    MALLOC(elf64_dynexarr, 1);
    MALLOC((*elf64_dynexarr).elf64_dynex, dyncount);
    elf64_dynexarr->size = dyncount;
    for(i = 0; i < dyncount; ++i)
    {
        elf64_dynexarr->elf64_dynex[i].elf64_dyn.d_tag = elf64_dyn[i].d_tag;
        elf64_dynexarr->elf64_dynex[i].elf64_dyn.d_un.d_val = elf64_dyn[i].d_un.d_val;  /* sizeof(Elf64_Xword) == sizeof(Elf64_Addr) */
    }



}
