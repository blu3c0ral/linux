#include <stdint.h>

/*
    Parser for 64bit ELF file
    If it did not succeed return value is non-zero
*/
uint8_t parse_elf64(char *file_ptr);