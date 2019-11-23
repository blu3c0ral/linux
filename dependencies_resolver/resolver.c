#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <features.h>


#include "resolver.h"

int
resolve_dependencies64(Elf64_DynEx * dyn_entries[], size_t dyne_size, char is_suid_sgid)
{
    Resolver_Data *resolver_data;
    size_t nleft;   /*  How much .so's left to find  */

    /*  Gather the data needed  */
    resolver_data = get_resolver_data(dyn_entries, dyne_size, is_suid_sgid);
    nleft = resolver_data->ndd_sos->nsa_size;

    /*
    *   Using the directories specified in the DT_RPATH dynamic section attribute of the binary if present 
    *   and DT_RUNPATH attribute does not exist.
    *   Use of DT_RPATH is deprecated.
    */
    if((resolver_data->dt_rpath!=NULL) && (resolver_data->dt_runpath==NULL))
    {
        nleft -= search_files_in_dirs(resolver_data->dt_rpath, resolver_data->rpath_size, resolver_data->ndd_sos); 
    }

    /*
    *   Using the environment variable LD_LIBRARY_PATH. Except if the executable is a set-user-ID/set-group-ID binary, 
    *   in which case it is ignored.
    */
    if ((resolver_data->env->ld_library_path != NULL) && (resolver_data->is_suid_sgid == 0) && (nleft > 0))
    {
        nleft -= search_files_in_dirs(resolver_data->env->ld_library_path, resolver_data->rpath_size, resolver_data->ndd_sos);
    }

    /*
    *   (ELF only) Using the directories specified in the DT_RUNPATH dynamic section attribute of the binary if present. 
    */
    if ((resolver_data->dt_runpath!=NULL) && (nleft > 0))
    {
        nleft -= search_files_in_dirs(resolver_data->dt_runpath, resolver_data->rpath_size, resolver_data->ndd_sos);
    }
}


Resolver_Data *
get_resolver_data(Elf64_DynEx * dyn_entries[], size_t dyne_size, char is_suid_sgid)
{
    size_t i;
    size_t res_ind = 0;
    size_t str_len;
    NeededString **ndd_strs = NULL;
    Resolver_Data *res_data;

    res_data = (Resolver_Data *) malloc(sizeof(Resolver_Data));
    if (res_data == NULL)
    {
        fprintf(stderr,"cannot allocate memory for %s in %s\n", "res_data", "get_resolver_data");
        return NULL;
    }

    /*  Initialize res_data */
    res_data->dt_rpath = NULL;
    res_data->dt_runpath = NULL;

    ndd_strs = (NeededString **) malloc(dyne_size * sizeof(NeededString *));
    if (ndd_strs == NULL)
    {
        fprintf(stderr,"cannot allocate memory for %s in %s\n", "ndd_strs[]", "get_resolver_data");
        return NULL;
    }

    /*  Switching between the relevant d_tags   */
    for(i = 0; i < dyne_size; ++i)
    {
        switch (dyn_entries[i]->elf64_dyn->d_tag)
        {
        case DT_NEEDED:
            str_len = strlen(dyn_entries[i]->string);
            ndd_strs[res_ind] = (NeededString *) malloc(sizeof(NeededString));
            if (ndd_strs[res_ind] == NULL)
            {
                fprintf(stderr,"cannot reallocate memory for %s in %s\n", "ndd_strs", "get_resolver_data");
                return NULL;
            }
            ndd_strs[res_ind]->ndd_str = (char *) malloc(str_len + 1);
            if (ndd_strs[res_ind]->ndd_str == NULL)
            {
                fprintf(stderr,"cannot reallocate memory for %s in %s\n", "ndd_strs->ndd_str", "get_resolver_data");
                return NULL;
            }
            memcpy(ndd_strs[res_ind]->ndd_str, dyn_entries[i]->string, str_len + 1);
            ndd_strs[res_ind]->fpath = NULL;
            ++res_ind;
            break;

        case DT_RPATH:
            res_data->dt_rpath = parse_delim_str(dyn_entries[i]->string, &(res_data->rpath_size));
            break;

        case DT_RUNPATH:
            res_data->dt_runpath = parse_delim_str(dyn_entries[i]->string, &res_data->runpath_size);
            break;
        
        default:
            break;
        }
    }
    ++res_ind;
    ndd_strs = (NeededString **) realloc(ndd_strs, res_ind * sizeof(NeededString *));
    if (ndd_strs == NULL)
    {
        fprintf(stderr,"cannot reallocate memory for %s in %s\n", "ndd_strs", "get_resolver_data");
        return NULL;
    }
    res_data->ndd_sos = (NeededStringChart *) malloc(sizeof(NeededStringChart));
    if (ndd_strs == NULL)
    {
        fprintf(stderr,"cannot allocate memory for %s in %s\n", "ndd_sos", "get_resolver_data");
        return NULL;
    }
    res_data->ndd_sos->ndd_str_arr = ndd_strs;
    res_data->ndd_sos->nsa_size = res_ind;

    /*  Set environment variables string    */
    res_data->env->ld_library_path = parse_delim_str(getenv("LD_LIBRARY_PATH"), &res_data->env->lib_size);
    res_data->env->ld_preload = parse_delim_str(getenv("LD_PRELOAD"), &res_data->env->pre_size);

    /*  Is setUID or setGID */
    res_data->is_suid_sgid = is_suid_sgid;

    return res_data;
}


/*
    parse_delim_str parse a delimited string into its individual strings
*/
char **
parse_delim_str(char *path_str, size_t *size)
{
    char delim = ':';
    char *tmp_char;
    char **result_strings;
    size_t lind = 0;
    size_t rind = 0;
    size_t count = 0;
    size_t i;
    char done = 0;
    
    if (!path_str)
    {
        return NULL;
    }
    /*  Count how many delimiters are in the string */
    tmp_char = path_str;
    while (*tmp_char)
    {
        if (*tmp_char == delim)
        {
            ++count;
        }
        ++tmp_char;
    }
    result_strings = (char **) malloc((count+1) * sizeof(char *));
    if (!result_strings)
    {
        fprintf(stderr,"cannot allocate memory for %s in %s\n", "result_strings", "parse_delim_str");
        return NULL;
    }
    count = 0;
    while (done == 0)
    {
        /*  rind is the right index traversing the string to the next delimiter or end  */
        while ((path_str[rind] != delim) && (path_str[rind] != '\0'))
        {
            ++rind;
        }
        if (path_str[rind] == '\0')
        {
            done = 1;
        }
        result_strings[count] = (char *) malloc(rind-lind+1);
        if (!result_strings)
        {
            fprintf(stderr,"cannot allocate memory for %s in %s\n", "result_strings", "parse_delim_str");
            return NULL;
        }
        for (i = lind; i <rind; ++i)
        {
            result_strings[count][i-lind] = path_str[i];
        }
        result_strings[count][rind-lind+1] = '\0';

        /*  For the next round, both indices are at the begining of the next string */
        lind = ++rind;
        ++count;
    }
    *size = count;
    return result_strings;
}


char *
concatenate_dir_filedir(char *dir, char *filename)
{
    size_t dir_size = strlen(dir);
    size_t fname_size = strlen(filename);
    size_t fp_size;
    char *file_path;
    if (dir[dir_size-1] == '/')
    {
        fp_size = dir_size + fname_size + 1;
    }
    else
    {
        fp_size = dir_size + fname_size + 2;
    }
    file_path = (char *) malloc(fp_size);
    if (!file_path)
    {
        fprintf(stderr,"cannot allocate memory for %s in %s\n", "file_path", "concatenate_dir_file");
        return NULL;
    }
    strcpy(file_path, dir);
    if (dir[dir_size-1] != '/')
    {
        strcat(file_path, "/");
    }
    strcat(file_path, filename);
    return file_path;
}


char *
search_file_in_dir(char *dir, char *filename)
{
    struct dirent *de;
    DIR *dr;
    char *tmp_dir;
    char *tmp_filepath;

    dr = opendir(dir);
    if (dr == NULL) 
    {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return NULL;
    }

    while((de = readdir(dr)) != NULL)
    {
        if(de->d_type == DT_DIR)
        {
            if(strcmp(".",de->d_name) == 0 || strcmp("..",de->d_name) == 0)
            {
                continue;
            }
            tmp_dir = concatenate_dir_filedir(dir, de->d_name);
            tmp_filepath = search_file_in_dir(tmp_dir, filename);
            free(tmp_dir);
            if (tmp_filepath)
            {
                closedir(dr);
                return tmp_filepath;
            }
        }
        else
        {
            if (strcmp(de->d_name, filename) == 0)
            {
                closedir(dr);
                return concatenate_dir_filedir(dir, filename);
            }
        }
        
    }
    closedir(dr);
    return NULL;
    
}


size_t
search_files_in_dirs(char **dirs, size_t dirs_num, NeededStringChart *ndd_chart)
{
    size_t found_count = 0;
    size_t i, j;

    for(i = 0; i < ndd_chart->nsa_size; ++i)
    {
        for(j = 0; (j < dirs_num) && (ndd_chart->ndd_str_arr[i]->fpath == NULL); ++j)
        {
            if (ndd_chart->ndd_str_arr[i]->fpath = search_file_in_dir(dirs[j], ndd_chart->ndd_str_arr[i]->ndd_str))
            {
                ++found_count;
            }
        }
    }

    return found_count;
}



long
file_size(int fd)
{
    __off_t size = 0;
    size = lseek(fd, 0, SEEK_END);
    if (size == -1) {
        return -1;
    }
    lseek(fd, 0, SEEK_SET);
    return (long)size;
}



char *
search_ldcache(NeededStringChart *ndd_chart)
{
    int fd;
    int OFLAGS = O_RDONLY;
    char *file_ptr;
    char *fpath;
    char *tmp_path;
    long fsize;
    size_t i;
    size_t tmp_len;

    fd  = open(LDCACHE_FILE, OFLAGS);

    fsize = file_size(fd);
    if (fsize == -1) 
    {
        fprintf("ERROR: %s size couldn't retrieved", LDCACHE_FILE);
        return NULL;
    }

    file_ptr = (char *)mmap(NULL, fsize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_ptr == NULL) 
    {
        fprintf("ERROR: %s couldn't be mapped to memory", LDCACHE_FILE);
        return NULL;
    }

    for(i = 0; i < ndd_chart->nsa_size; ++i)
    {
        tmp_len = strlen(ndd_chart->ndd_str_arr[i]);
        tmp_path = (char *) malloc()
        fpath = (char *) memmem(file_ptr, fsize, tmp_path, tmp_len);
    }
}