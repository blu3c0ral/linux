#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "resolver.h"

int
resolve_dependencies64(Elf64_DynEx * dyn_entries[], size_t dyne_size)
{
    Resolver_Data *resolver_data;

    resolver_data = get_resolver_data(dyn_entries, dyne_size);


}


Resolver_Data *
get_resolver_data(Elf64_DynEx * dyn_entries[], size_t dyne_size)
{
    size_t i;
    size_t res_ind = -1;
    size_t str_len;
    char **res_string = NULL;
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

    res_string = (char **) malloc(dyne_size * sizeof(char *));
    if (res_string == NULL)
    {
        fprintf(stderr,"cannot allocate memory for %s in %s\n", "res_string", "get_resolver_data");
        return NULL;
    }

    /*  Switching between the relevant d_tags   */
    for(i = 0; i < dyne_size; ++i)
    {
        switch (dyn_entries[i]->elf64_dyn->d_tag)
        {
        case DT_NEEDED:
            ++res_ind;
            str_len = strlen(dyn_entries[i]->string);
            res_string[res_ind] = (char *) malloc(str_len + 1);
            if (res_string == NULL)
            {
                fprintf(stderr,"cannot reallocate memory for %s in %s\n", "res_string", "get_resolver_data");
                return NULL;
            }
            memcpy(res_string[res_ind], dyn_entries[i]->string, str_len + 1);
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

    res_string = (char **) realloc(res_string, res_ind * sizeof(char *));
    if (res_string == NULL)
    {
        fprintf(stderr,"cannot reallocate memory for %s in %s\n", "res_string", "get_resolver_data");
        return NULL;
    }
    res_data->dependencies_strings = res_string;
    res_data->dep_str_size = res_ind;

    /*  Set environment variables string    */
    res_data->env->ld_library_path = parse_delim_str(getenv("LD_LIBRARY_PATH"), &res_data->env->lib_size);
    res_data->env->ld_preload = parse_delim_str(getenv("LD_PRELOAD"), &res_data->env->pre_size);

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
                closedir(dir);
                return tmp_filepath;
            }
        }
        else
        {
            if (strcmp(de->d_name, filename) == 0)
            {
                closedir(dir);
                return concatenate_dir_filedir(dir, filename);
            }
        }
        
    }
    closedir(dir);
    return NULL;
    
}