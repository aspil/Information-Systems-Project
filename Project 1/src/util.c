#include "../include/util.h"

void strip_ext(char *fname) {
    char *end = fname + strlen(fname);
    while (end > fname && *end != '.')
        --end;
    
    if (end > fname)
        *end = '\0';
    return;
}