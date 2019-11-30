#include <stdint.h>

typedef uint64_t FP_Off;

typedef struct 
{
    char *_f_ptr;
    size_t _f_size;
} File_Ptr;

#define PRINT_ERROR(msg) fprintf(stderr, "%s\n", (msg));


File_Ptr *f_open(const char *f_path);

size_t f_read(char *dst, File_Ptr *f_ptr, FP_Off f_off, size_t r_size);


