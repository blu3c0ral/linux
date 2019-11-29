#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void error_exit(const char *errmsg) {
    fprintf(stderr, "%s\n", errmsg);
    exit(1);
}

long file_size(int fd)
{
    __off_t size = 0;
    size = lseek(fd, 0, SEEK_END);
    if (size == -1) {
        return -1;
    }
    lseek(fd, 0, SEEK_SET);
    return (long)size;
}