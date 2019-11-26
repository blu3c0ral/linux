#include <elf.h>

int8_t get_elf_class(char *file_ptr);

/*
    Get the 64bit ELF header struct from a file descriptor
*/
Elf64_Ehdr * get_Ehdr64(char *file_ptr);

/*
    Get the 32bit ELF header struct from a file descriptor
*/
Elf32_Ehdr * get_Ehdr32(char *file_ptr);

/*
    f
*/
char *get_file_ptr(const char *fpath);
