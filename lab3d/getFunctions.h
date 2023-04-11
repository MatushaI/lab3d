#ifndef getFunctions_h
#define getFunctions_h

#include <stdio.h>
#include "TableHashHD.h"

FILE* getFile(const char* invite, char choose[4], int *check);
char *readString(FILE *file);
int getIntUnsignt(char *number);
char *myreadline(const char* invite);
int importFile(TableHHD *table, TableHash *search);
FILE *importBinaryFile(const char* invite, char choose[4]);

#endif 
