#include <stdint.h>
#include <stdio.h>



/*  Elf64_DynEx struct is extending the Elf64_Dyn for those which contains a string    */
typedef struct
{
    Elf64_Dyn * elf64_dyn;
    char *      string;
} Elf64_DynEx;



typedef struct 
{
    Elf64_DynEx * elf64_dynex;
    size_t size;
} Elf64_DynExArr;



/*
    Parser for 64bit ELF file
    If it did not succeed return value is non-zero
*/
uint8_t parse_elf64(char *file_ptr);



Elf64_Shdr *get_elf64_shdr(char *file_ptr, Elf64_Ehdr *elf64_ehdr);



/*
*   ldd_data function returns a pointer to struct containing 
*/
extern Elf64_DynExArr * ldd_data(char *file_path);