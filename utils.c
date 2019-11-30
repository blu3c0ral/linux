#include <stdio.h>
#include <stdlib.h>

void error_exit(const char *errmsg) {
    fprintf(stderr, "%s\n", errmsg);
    exit(1);
}

void error_msg(const char *errmsg) {
    fprintf(stderr, "%s\n", errmsg);
}
