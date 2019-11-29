#include <stdlib.h>
#include <unistd.h>

#include "fileutils.h"
#include "memory.h"

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

File_Ptr *f_open(const char *f_path)
{
    int fd;
    int OFLAGS = O_RDONLY;
    long fsize;
    File_Ptr *file_ptr;

    MALLOC(file_ptr, 1);

    fd  = open(f_path, OFLAGS);

    file_ptr->_f_size = file_size(fd);
    if (file_ptr->_f_size == -1) 
    {
        return NULL;
    }

    file_ptr->_f_ptr = (char *)mmap(NULL, file_ptr->_f_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_ptr->_f_ptr == NULL) 
    {
        return NULL;
    }

    close(fd);

    return file_ptr;
}
