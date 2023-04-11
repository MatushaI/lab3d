#ifndef dialog_h
#define dialog_h

#include <stdio.h>
#include "TableHashHD.h"

int dialog(const char *msgs[], int N, int *ifPagePrev);
int deleteKey_In(TableHHD *table);
int deleteOldVersions_In(TableHHD *table);

int addInfo_In(TableHHD *table, TableHash *search);
int delete_In(TableHHD *table, TableHash *search);
int search_In(TableHHD *table, TableHash *search);
int printTable_In(TableHHD *table, TableHash *search);

void printCat(void);

#endif 
