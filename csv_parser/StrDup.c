#include <string.h>
#include "csv.h"
char *StringDup(char *src) {
    char *dst = malloc(strlen (src) + 1);  
    if (dst == NULL) return NULL;         
    strcpy(dst, src);                      
    return dst;                            
}