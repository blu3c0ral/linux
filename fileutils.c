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

        if (f_off > f_ptr->_f_size)
    {
        error_msg("error - offset is not within the file\n");
        return 0;
    }

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


char *f_str_read(File_Ptr *f_ptr, FP_Off f_off)
{
    char *str;
    size_t bytes_count;
    size_t bytes_cap;


    if (f_off > f_ptr->_f_size)
    {
        error_msg("error - offset is not within the file\n");
        return NULL;
    }

    MALLOC(str, 15);
    bytes_count = 0;
    while ((str[bytes_count] != '\0') && (f_off + bytes_count <= f_ptr->_f_size))
    {
        str[bytes_count] = f_ptr->_f_ptr[f_off + bytes_count];
        ++bytes_count;
        if (bytes_count == bytes_cap)
        {
            REALLOC(str, bytes_cap * 2);
        }
    }
    REALLOC(str, bytes_count);
    return str;    
}
