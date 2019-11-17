#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resolver.h"

int
resolve_dependencies64(Elf64_DynEx * dyn_entries[], size_t dyne_size)
{
    Resolver_Data *needed_so;

    needed_so = get_resolver_data(dyn_entries, dyne_size);

}

Resolver_Data *
get_resolver_data(Elf64_DynEx * dyn_entries[], size_t dyne_size)
{
    size_t i;
    size_t res_ind = -1;
    size_t str_len;
    char *res_string = NULL;
    Resolver_Data *res_data;

    res_data = malloc(sizeof(Resolver_Data));
    if (res_data == NULL)
    {
        return NULL;
    }

    res_string = malloc(dyne_size * sizeof(Elf64_DynEx));
        if (res_string == NULL)
    {
        return NULL;
    }

    for(i = 0; i < dyne_size; ++i)
    {
        switch (dyn_entries[i]->elf64_dyn->d_tag)
        {
        case DT_NEEDED:
            ++res_ind;
            str_len = strlen(dyn_entries[i]->string);
            res_string[res_ind] = malloc(str_len + 1);
            memcpy(res_string[res_ind], dyn_entries[i]->string, str_len + 1);
            break;

        case DT_RPATH:
            str_len = strlen(dyn_entries[i]->string);
            res_data->dt_rpath = malloc(str_len + 1);
            memcpy(res_data->dt_rpath, dyn_entries[i]->string, str_len + 1);
            break;

        case DT_RUNPATH:
            str_len = strlen(dyn_entries[i]->string);
            res_data->dt_runpath = malloc(str_len + 1);
            memcpy(res_data->dt_runpath, dyn_entries[i]->string, str_len + 1);
            break;
        
        default:
            break;
        }
    }
    res_data->dependencies_strings = res_string;

    return res_data;
}