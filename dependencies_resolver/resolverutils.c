#include "resolverutils.h"

#include <stdio.h>

#include "../memory.h"

char *addPrefix(char *string, char *prefix)
{
    char *resStr;
    size_t stringLen;
    size_t prefixLen;

    stringLen = strlen(string);
    prefixLen = strlen(prefix);
    MALLOC(resStr, stringLen + prefixLen + 1);
    
    memcpy()
}