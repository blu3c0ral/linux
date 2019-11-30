#include <elf.h>

#include "../fileutils.h"

int8_t get_elf_class(File_Ptr *file_ptr);

/*
    Get the 64bit ELF header struct from a file descriptor
*/
Elf64_Ehdr *get_Ehdr64(File_Ptr *f_ptr);

/*
    Get the 32bit ELF header struct from a file descriptor
*/
Elf32_Ehdr *get_Ehdr32(File_Ptr *f_ptr);
