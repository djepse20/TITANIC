#include "csv_parser/csv.h"
#include <stdlib.h>

int main()
{
    int err = 0, done = 0;
    char *x;
    FILE* y = fopen("test.csv","r");
    x = fread_csv_line(y,400,&done,&err);
    printf("%s",x);
    return 0;
}