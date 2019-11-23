#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resolver.h"

#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>


/*
void main(void)
{
    char *  sent = "one_sent:two_sent:three_sent";
    char *  paths = malloc(strlen(sent)+1);
    char ** results;
    size_t res_size;
    size_t i;

    memcpy(paths, sent, strlen(sent)+1);

    results = parse_delim_str(paths, &res_size);

    for(i = 0; i < res_size; ++i)
    {
        printf("%s\n", results[i]);
    }
}
*/

void printdir(char *dir, int depth)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if((dp = opendir(dir)) == NULL) {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return;
    }
    chdir(dir);
    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)) {
            /* Found a directory, but ignore . and .. */
            if(strcmp(".",entry->d_name) == 0 ||
                strcmp("..",entry->d_name) == 0)
                continue;
            printf("%*s%s/\n",depth,"",entry->d_name);
            /* Recurse at a new indent level */
            printdir(entry->d_name,depth+4);
        }
        else printf("%*s%s\n",depth,"",entry->d_name);
    }
    chdir("..");

    closedir(dp);
}

int main()
{
    //printf("Directory scan of /home:\n");
    //printdir("/home",0);
    //printf("done.\n");
    char *result;
    printf("%s\n", ((result=search_file_in_dir("/home", "resolver.c"))==NULL)?"":result);
    free(result);
    exit(0);
}