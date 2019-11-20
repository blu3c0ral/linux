#include <stdio.h>

#include <elf.h>

/*  Elf64_DynEx struct is extending the Elf64_Dyn for those which contains a string    */
typedef struct
{
    Elf64_Dyn * elf64_dyn;
    char *      string;
} Elf64_DynEx;


typedef struct {
    char ** ld_library_path;
    char ** ld_preload;

    size_t  lib_size;
    size_t  pre_size;
} RD_Env;

/*  Should contain everything needed for the so lookup  */
typedef struct
{
    char ** dependencies_strings;
    char ** dt_rpath;
    char ** dt_runpath;

    size_t  dep_str_size;
    size_t  rpath_size;
    size_t  runpath_size;

    RD_Env *env;
} Resolver_Data;



#define MAX_STACK_SIZE 1000
#define s_init(arr_stack) \
    { \
        for(size_t i = 0; i < MAX_STACK_SIZE; ++ i) \
        { \
            (arr_stack)[i] = NULL; \
        } \
    }
#define pop(arr_stack, count) \
    { \
        if ((count) == 0) \
        { \
            fprintf(stderr,"stack is empty\n"); \
        } \
        else \
        { \
            for(size_t i=0; i < (count); ++i) \
            { \
                (arr_stack)[i] = (arr_stack)[i+1]; \
            } \
            (arr_stack)[(count)-1] = NULL; \
            --(count); \
        } \
    }
#define push(arr_stack, de, count) \
    { \
        if ((count) == MAX_STACK_SIZE) \
        { \
            fprintf(stderr,"cannot push more items\n"); \
        } \
        else \
        { \
            (arr_stack)[(count)++] = de; \
        } \
    }

/*  Basically gcc will add the next file types defines with this macro defined */
/*
#define _BSD_SOURCE
*/


/*
 * File types
 */
/*
#define	DT_UNKNOWN	 0
#define	DT_FIFO		 1
#define	DT_CHR		 2
#define	DT_DIR		 4
#define	DT_BLK		 6
#define	DT_REG		 8
#define	DT_LNK		10
#define	DT_SOCK		12
*/


int
resolve_dependencies64(Elf64_DynEx * dyn_entries[], size_t dyne_size);



Resolver_Data *
get_resolver_data(Elf64_DynEx * dyn_entries[], size_t dyne_size);



char **
parse_delim_str(char *path_str, size_t *size);



char *
search_file_in_dir(char *dir, char *filename);