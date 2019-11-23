#include <stdio.h>

#include <elf.h>

#include "memory.h"

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

typedef struct {
    char * ndd_str;
    char * fpath;
} NeededString;

typedef struct {
    NeededString ** ndd_str_arr;
    
    size_t nsa_size;    /*  I know...   */
} NeededStringChart;

/*  Should contain everything needed for the so lookup  */
typedef struct
{
    NeededStringChart * ndd_sos;

    char ** dt_rpath;
    char ** dt_runpath;

    size_t  rpath_size;
    size_t  runpath_size;

    RD_Env *env;

    char is_suid_sgid;
} Resolver_Data;



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


/*  ld.cache file location  */
#define LDCACHE_FILE "/etc/ld.so.cache"
#define _GNU_SOURCE




int
resolve_dependencies64(Elf64_DynEx * dyn_entries[], size_t dyne_size, char is_suid_sgid);



Resolver_Data *
get_resolver_data(Elf64_DynEx * dyn_entries[], size_t dyne_size, char is_suid_sgid);



char **
parse_delim_str(char *path_str, size_t *size);



char *
concatenate_dir_filedir(char *dir, char *filename);



char *
search_file_in_dir(char *dir, char *filename);



size_t
search_files_in_dirs(char **dirs, size_t dirs_num, NeededStringChart *ndd_chart);



size_t
search_ldcache(NeededStringChart *ndd_chart);