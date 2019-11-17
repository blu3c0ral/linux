#include "elf64parser.h"

#include <stdint.h>

#include <elf.h>

#include "elfutils.h"

uint8_t 
parse_elf64(char *file_ptr) 
{
    Elf64_Ehdr * elf64_ehdr;

    elf64_ehdr = get_Ehdr64(file_ptr);
}