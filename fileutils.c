#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "fileutils.h"
#include "memory.h"
#include "utils.h"



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
    if (file_ptr->_f_size < 0) 
    {
        error_msg("error - can't get the file size\n");
        return NULL;
    }

    file_ptr->_f_ptr = (char *)mmap(NULL, file_ptr->_f_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_ptr->_f_ptr == NULL) 
    {
        error_msg("error - can't map the file\n");
        return NULL;
    }

    close(fd);

    return file_ptr;
}


size_t f_read(char *dst, File_Ptr *f_ptr, FP_Off f_off, size_t r_size)
{
    char *r_bytes;
    size_t c_read;
    uint64_t i;

    if (r_size <= f_ptr->_f_size - f_off)
    {
        c_read = r_size;
    }
    else
    {
        error_msg("error - requested reading size is exceeding the file boundaries\n");
        c_read = f_ptr->_f_size - f_off;
    }

    for(i = 0; i < c_read; ++i)
    {
        dst[i] = f_ptr->_f_ptr[i];
    }

    return c_read;
}
