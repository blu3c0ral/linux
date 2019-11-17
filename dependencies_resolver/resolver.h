#include <stdio.h>

#include <elf.h>


/*  Elf64_DynEx struct is extending the Elf64_Dyn for those which contains a string    */
typedef struct
{
    Elf64_Dyn * elf64_dyn;
    char *      string;
} Elf64_DynEx;


typedef struct
{
    char *  dependencies_strings;
    char *  dt_rpath;
    char *  dt_runpath;
} Resolver_Data;





int
resolve_dependencies64(Elf64_DynEx * dyn_entries[], size_t dyne_size);



char *
get_dependencies_list64(Elf64_DynEx * dyn_entries[], size_t dyne_size);