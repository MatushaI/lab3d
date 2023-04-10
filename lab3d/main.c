#include <stdio.h>
#include <stdlib.h>
#include "getFunctions.h"
#include "TableHashHD.h"

int main(void) {
    
    /*
    unsigned int size = 10;
    unsigned long long allOffset = sizeof(unsigned long long) + sizeof(TitleHashHD)*size + sizeof(unsigned int);
    
    FILE *file = getFile("Введите название файла > ", "w+r", NULL);
    fwrite(&allOffset, sizeof(unsigned long long), 1, file);
    fwrite(&size, sizeof(unsigned int), 1, file);
    
    TitleHashHD *titleHash = NULL;
    titleHash = calloc(size, sizeof(TitleHashHD));
    for (int i = 0; i < size; i++) {
        titleHash[i].KeySpaceOffset = 0;
    }
    
    fwrite(titleHash, sizeof(TitleHashHD), size, file);
    fclose(file);
    */
    
    FILE *file1 = getFile("Введите название файла > ", "r+", NULL);
    TableHHD *table = importTableHHD(file1);
    TableHash *search = createTH(10);
    char *key = myreadline("Ключ > ");
    char *info = myreadline("Инфо > ");
    addInfoHHD(table, info, key);
    printTableHHD(table);
    char *src = myreadline("search > ");
    search = searchKeyVersionTH(table, search, src, 2);
    //deleteOldVersionsHHD(table, del);
    printTH(search);
    
    fseek(file1, 0, SEEK_SET);
    fwrite(&table->allOffset, sizeof(unsigned long long), 1, file1);
    
    return 0;
}
