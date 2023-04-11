#include <stdio.h>
#include <stdlib.h>
#include "getFunctions.h"
#include "TableHashHD.h"
#include "dialog.h"

int main(void) {
    
    FILE *file = importBinaryFile("Введите название файла, содержащего таблицу\n", "r+");
    
    if(!file) {
        return 0;
    }
    
    TableHHD *table = importTableHHD(file);
    TableHash *search = createTH(10);
    
    const char *msgs[] = {"0. Выход", "1. Добавить элемент", "2. Поиск", "3. Удаление", "4. Печать таблицы", "5. Импорт"};
    const int Nmsgs = sizeof(msgs) / sizeof(msgs[0]);
    
    int (*func[])(TableHHD *, TableHash *) = {NULL, addInfo_In, search_In, delete_In, printTable_In, importFile};
    
    int rc;
    while((rc = dialog(msgs, Nmsgs, NULL))){
        if(!func[rc](table, search)) 
            break;
    }
    
    exportTableHHD(table);
    clearTableHash(search);
    
    printf("\n*** Программа завершена ***\n");
    
    return 0;
}


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
