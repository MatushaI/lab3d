#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "TableHashHD.h"

TableHHD *importTableHHD(FILE *file) {
    
    TableHHD *table = calloc(1, sizeof(TableHHD));
    unsigned long freadCheck = 0;
    
    fseek(file, 0, SEEK_SET);
    table->file = file;
    freadCheck += fread(&table->allOffset, sizeof(unsigned long long), 1, file);
    freadCheck += fread(&table->sizeTitle, sizeof(unsigned int), 1, file);
    if(!freadCheck) {
        free(table);
        table = NULL;
    } 
    
    return table;
}

int exportTableHHD(TableHHD *table) {
    fseek(table->file, 0, SEEK_SET);
    fwrite(&table->allOffset, sizeof(unsigned long long), 1, table->file);
    fclose(table->file);
    free(table);
    return 0;
}

int getIndex(char *string, int maxSize) {
    
    if(string == NULL) {
        return -1;
    }
    
    long hash = 0;
    int index;
    double intpart = 1;
    for (int i = 0; i < strlen(string); i++) {
        hash = (hash<<5) - hash + string[i];
    }
    double a = 0.61803398;
    index = round((maxSize - 1)*modf(hash*a, &intpart));
    return index;
}

int addInfoHHD(TableHHD *table, char *info, char *key) {
    
    if(!info || !key ) {
        if(key) {
            free(key);
        }
        if(info) {
            free(info);
        }
        return 1;
    }
    
    int index = getIndex(key, table->sizeTitle);
    TitleHashHD titleHash;
    KeySpaceHHD keySpace;
    char *keyTable = NULL;
    unsigned long long offset;
    unsigned long long itemOffset;
    
    fseek(table->file, sizeof(unsigned int) + sizeof(unsigned long long) + index*sizeof(TitleHashHD), SEEK_SET);
    fread(&titleHash, sizeof(TitleHashHD), 1, table->file);
    offset = titleHash.KeySpaceOffset;
    while (offset) {
        fseek(table->file, offset, SEEK_SET);
        fread(&keySpace, sizeof(KeySpaceHHD), 1, table->file);
        fseek(table->file, keySpace.keyOffset, SEEK_SET);
        keyTable = calloc(keySpace.keyLenght, sizeof(char));
        fread(keyTable, sizeof(char), keySpace.keyLenght, table->file);
        if(!strcmp(key, keyTable)) {
            free(keyTable);
            break;
        }
        free(keyTable);
        offset = keySpace.nextOffset;
    }
    
    if(offset == 0) {
        fseek(table->file, table->allOffset, SEEK_SET);
        fwrite(key, sizeof(char), strlen(key) + 1, table->file);
        memset(&keySpace, 0, sizeof(KeySpaceHHD));
        keySpace.keyLenght = (int) strlen(key) + 1;
        keySpace.keyOffset = table->allOffset;
        table->allOffset = table->allOffset + keySpace.keyLenght;
        keySpace.ItemOffset = 0;
        keySpace.nextOffset = titleHash.KeySpaceOffset;
        fseek(table->file, table->allOffset, SEEK_SET);
        fwrite(&keySpace, sizeof(KeySpaceHHD), 1, table->file);
        offset = table->allOffset;
        titleHash.KeySpaceOffset = offset;
        table->allOffset = table->allOffset + sizeof(KeySpaceHHD);
    }
    
    ItemHHD item;
    ItemHHD nextItem;
    
    if(keySpace.ItemOffset == 0) {
        item.version = 1;
        item.nextOffset = 0;
    } else {
        fseek(table->file, keySpace.ItemOffset, SEEK_SET);
        fread(&nextItem, sizeof(ItemHHD), 1, table->file);
        item.version = nextItem.version + 1;
        item.nextOffset = keySpace.ItemOffset;
    }
    
    item.lenghtInfo = (int) strlen(info) + 1;
    item.infoOffset = table->allOffset;
    fseek(table->file, table->allOffset, SEEK_SET);
    fwrite(info, sizeof(char), strlen(info) + 1, table->file);
    table->allOffset = table->allOffset + item.lenghtInfo;
    
    itemOffset = table->allOffset;
    fseek(table->file, table->allOffset, SEEK_SET);
    fwrite(&item, sizeof(ItemHHD), 1, table->file);
    table->allOffset = table->allOffset + sizeof(ItemHHD);
    
    keySpace.ItemOffset = itemOffset;
    fseek(table->file, offset, SEEK_SET);
    fwrite(&keySpace, sizeof(KeySpaceHHD), 1, table->file);
    
    if(titleHash.KeySpaceOffset == keySpace.nextOffset) {
        titleHash.KeySpaceOffset = offset;
    }
    
    fseek(table->file, sizeof(unsigned int) + sizeof(unsigned long long) + index*sizeof(TitleHashHD), SEEK_SET);
    fwrite(&titleHash, sizeof(TitleHashHD), 1, table->file);
    
    free(key);
    free(info);
    
    return 0;
}

void printTableHHD(TableHHD *table) {
    
    TitleHashHD titleHash;
    KeySpaceHHD keySpace;
    ItemHHD item;
    unsigned long long keySpaceOffset = 0;
    unsigned long long itemOffset = 0;
    unsigned long long titleHashOffset = sizeof(unsigned long long) + sizeof(unsigned int);
    char *key = NULL;
    char *info = NULL;
    
    for (int i = 0; i < table->sizeTitle; i++) {
        fseek(table->file, titleHashOffset, SEEK_SET); 
        fread(&titleHash, sizeof(TitleHashHD), 1, table->file);
        keySpaceOffset = titleHash.KeySpaceOffset;
        while (keySpaceOffset) {
            printf("\n");
            fseek(table->file, keySpaceOffset, SEEK_SET);
            fread(&keySpace, sizeof(KeySpaceHHD), 1, table->file);
            key = calloc(keySpace.keyLenght, sizeof(char));
            fseek(table->file, keySpace.keyOffset, SEEK_SET);
            fread(key, sizeof(char), keySpace.keyLenght, table->file);
            printf("Ключ: %s\n", key);
            free(key);
            itemOffset = keySpace.ItemOffset;
            while (itemOffset) {
                fseek(table->file, itemOffset, SEEK_SET);
                fread(&item, sizeof(ItemHHD), 1, table->file);
                info = calloc(item.lenghtInfo, sizeof(char));
                fseek(table->file, item.infoOffset, SEEK_SET);
                fread(info, sizeof(char), item.lenghtInfo, table->file);
                printf("Версия: %d, Информация: %s\n", item.version, info);
                free(info);
                itemOffset = item.nextOffset;
            }
            keySpaceOffset = keySpace.nextOffset;
        }
        titleHashOffset += sizeof(TitleHashHD);
    }
}

int deleteKeyHHD(TableHHD *table, char *key) {
    
    if(!key) {
        return 1;
    }
    
    int index = getIndex(key, table->sizeTitle);
    TitleHashHD titleHash;
    KeySpaceHHD keySpace;
    char *keyTable = NULL;
    unsigned long long offset;
    unsigned long long prevOffset = 0;
    
    fseek(table->file, sizeof(unsigned int) + sizeof(unsigned long long) + index*sizeof(TitleHashHD), SEEK_SET);
    fread(&titleHash, sizeof(TitleHashHD), 1, table->file);
    offset = titleHash.KeySpaceOffset;
    
    while (offset) {
        fseek(table->file, offset, SEEK_SET);
        fread(&keySpace, sizeof(KeySpaceHHD), 1, table->file);
        fseek(table->file, keySpace.keyOffset, SEEK_SET);
        keyTable = calloc(keySpace.keyLenght, sizeof(char));
        fread(keyTable, sizeof(char), keySpace.keyLenght, table->file);
        if(!strcmp(key, keyTable)) {
            free(keyTable);
            break;
        }
        free(keyTable);
        prevOffset = offset;
        offset = keySpace.nextOffset;
        
    }
    if(!offset) {
        return 1;
    }
    
    if(!prevOffset) {
        titleHash.KeySpaceOffset = keySpace.nextOffset;
        fseek(table->file, sizeof(unsigned int) + sizeof(unsigned long long) + index*sizeof(TitleHashHD), SEEK_SET);
        fwrite(&titleHash, sizeof(TitleHashHD), 1, table->file);
    } else {
        offset = keySpace.nextOffset;
        fseek(table->file, prevOffset, SEEK_SET);
        fread(&keySpace, sizeof(KeySpaceHHD), 1, table->file);
        keySpace.nextOffset = offset;
        fseek(table->file, prevOffset, SEEK_SET);
        fwrite(&keySpace, sizeof(KeySpaceHHD), 1, table->file);
    }
    
    free(key);
    
    return 0;
}

int deleteOldVersionsHHD(TableHHD *table, char *key) {
    
    if(!key) {
        return 1;
    }
    
    int index = getIndex(key, table->sizeTitle);
    TitleHashHD titleHash;
    KeySpaceHHD keySpace;
    ItemHHD item;
    char *keyTable = NULL;
    unsigned long long offset;
    fseek(table->file, sizeof(unsigned int) + sizeof(unsigned long long) + index*sizeof(TitleHashHD), SEEK_SET);
    fread(&titleHash, sizeof(TitleHashHD), 1, table->file);
    offset = titleHash.KeySpaceOffset;
    
    while (offset) {
        fseek(table->file, offset, SEEK_SET);
        fread(&keySpace, sizeof(KeySpaceHHD), 1, table->file);
        keyTable = calloc(keySpace.keyLenght, sizeof(char));
        fseek(table->file, keySpace.keyOffset, SEEK_SET);
        fread(keyTable, sizeof(char), keySpace.keyLenght, table->file);
        if(!strcmp(key, keyTable )) {
            free(keyTable);
            break;
        }
        free(keyTable);
        offset = keySpace.nextOffset;
    }
    
    if(!offset) {
        return 1;
    } else {
        fseek(table->file, keySpace.ItemOffset, SEEK_SET);
        fread(&item, sizeof(ItemHHD), 1, table->file);
        item.nextOffset = 0;
        fseek(table->file, keySpace.ItemOffset, SEEK_SET);
        fwrite(&item, sizeof(ItemHHD), 1, table->file);
    }
    
    free(key);
    
    return 0;
}

void clearTableTHNoMain(TableHash *table) {
        KeyspaceHash *ks = NULL;
        KeyspaceHash *prev = NULL;
        Item *item = NULL;
        Item *del = NULL;
        
        if(table) {
            for (int i = 0; i < table->maxSize; i++) {
                ks = table->th[i].ks;
                while (ks) {
                    prev = ks;
                    item = ks->item;
                    while (item) {
                        del = item;
                        free(item->info);
                        item = item->next;
                        free(del);
                    }
                    free(ks->key);
                    ks = ks->next;
                    free(prev);
                }
            }
        }
    ks = table->th[0].ks = NULL;
}

TableHash *createTH(int size) {
    if(size <= 0) {
        return NULL;
    }
    TableHash *table = calloc(1, sizeof(TableHash));
    table->maxSize = size;
    table->th = calloc(size, sizeof(TableHash));
    for (int i = 0; i < size; i++) {
        table->th[i].ks = NULL;
    }
    return table;
}

TableHash *searchKeyTH(TableHHD *table, TableHash *search, char *key) {
    
    if(!key) {
        clearTableTHNoMain(search);
        return NULL;
    }
    
    if(search) {
        clearTableTHNoMain(search);
    }
    
    Item *newItem = NULL;
    
    int index = getIndex(key, table->sizeTitle);
    TitleHashHD titleHash;
    KeySpaceHHD keySpace;
    ItemHHD itemHD;
    unsigned long long offset;
    unsigned long long itemOffset;
    char *keyTable;
    fseek(table->file, sizeof(unsigned int) + sizeof(unsigned long long) + index*sizeof(TitleHashHD), SEEK_SET);
    fread(&titleHash, sizeof(TitleHashHD), 1, table->file);
    offset = titleHash.KeySpaceOffset;
    
    while (offset) {
        fseek(table->file, offset, SEEK_SET);
        fread(&keySpace, sizeof(KeySpaceHHD), 1, table->file);
        keyTable = calloc(keySpace.keyLenght, sizeof(char));
        fseek(table->file, keySpace.keyOffset, SEEK_SET);
        fread(keyTable, sizeof(char), keySpace.keyLenght, table->file);
        if(!strcmp(key, keyTable )) {
            free(keyTable);
            break;
        }
        free(keyTable);
        offset = keySpace.nextOffset;
    }
    
    if(!offset) {
        free(key);
        search = NULL;
        return NULL;
    }
    
    KeyspaceHash *ksNew = calloc(1, sizeof(KeyspaceHash));
    ksNew->item = NULL;
    ksNew->key = strdup(key);
    ksNew->next = NULL;
    search->th[0].ks = ksNew;
    
    itemOffset = keySpace.ItemOffset;
    
    while (itemOffset) {
        fseek(table->file, itemOffset, SEEK_SET);
        fread(&itemHD, sizeof(itemHD), 1, table->file);
        keyTable = calloc(itemHD.lenghtInfo, sizeof(char));
        fseek(table->file, itemHD.infoOffset, SEEK_SET);
        fread(keyTable, sizeof(char), itemHD.lenghtInfo, table->file);
        
        newItem = calloc(1, sizeof(Item));
        newItem->version = itemHD.version;
        newItem->info = strdup(keyTable);
        newItem->next = NULL;
        
        if(!ksNew->item) {
            ksNew->item = newItem;
        } else {
            newItem->next = ksNew->item;
            ksNew->item = newItem;
        }
        free(keyTable);
        itemOffset = itemHD.nextOffset;
    }
    free(key);
    return search;
}

int printTH(TableHash *table) {
    if(!table) {
        return 1;
    }
    KeyspaceHash *ks = NULL;
    Item *item = NULL;
    for (int i = 0; i < table->maxSize; i++) {
        ks = table->th[i].ks;
        while (ks) {
            printf("\n");
            printf("Ключ: %s\n", ks->key);
            item = ks->item;
            while (item) {
                printf("Версия: %d, информация: %s\n", item->version, item->info);
                item = item->next;
            }
            ks = ks->next;
        }
    }
    return 0;
}

TableHash *searchKeyVersionTH(TableHHD *table, TableHash *search, char *key, int version) {
    
    if(!key) {
        clearTableTHNoMain(search);
        return NULL;
    }
    
    if(search) {
        clearTableTHNoMain(search);
    }
    
    Item *newItem = NULL;
    
    int index = getIndex(key, table->sizeTitle);
    KeySpaceHHD keySpace;
    TitleHashHD titleHash;
    ItemHHD itemHD;
    char *keyTable = NULL;
    unsigned long long itemOffset;
    unsigned long long offset;
    
    fseek(table->file, sizeof(int) + sizeof(unsigned long long) + index*sizeof(TitleHashHD), SEEK_SET);
    fread(&titleHash, sizeof(TitleHash), 1, table->file);
    offset = titleHash.KeySpaceOffset;
    
    while (offset) {
        fseek(table->file, offset, SEEK_SET);
        fread(&keySpace, sizeof(KeySpaceHHD), 1, table->file);
        keyTable = calloc(keySpace.keyLenght, sizeof(char));
        fseek(table->file, keySpace.keyOffset, SEEK_SET);
        fread(keyTable, sizeof(char), keySpace.keyLenght, table->file);
        if(!strcmp(keyTable, key)) {
            free(keyTable);
            break;
        }
        free(keyTable);
        offset = keySpace.nextOffset;
    }
    
    if(!offset) {
        free(key);
        search = NULL;
        return NULL;
    }
    
    KeyspaceHash *ksNew = calloc(1, sizeof(KeyspaceHash));
    ksNew->item = NULL;
    ksNew->key = strdup(key);
    free(key);
    ksNew->next = NULL;
    search->th[0].ks = ksNew;
    
    itemOffset = keySpace.ItemOffset;
    
    while (itemOffset) {
        fseek(table->file, itemOffset, SEEK_SET);
        fread(&itemHD, sizeof(ItemHHD), 1, table->file);
        if(itemHD.version == version) {
            keyTable = calloc(itemHD.lenghtInfo, sizeof(char));
            fseek(table->file, itemHD.infoOffset, SEEK_SET);
            fread(keyTable, sizeof(char), itemHD.lenghtInfo, table->file);
                
            newItem = calloc(1, sizeof(Item));
            newItem->version = itemHD.version;
            newItem->info = strdup(keyTable);
            newItem->next = NULL;
            ksNew->item = newItem;
            break;
        }
        itemOffset = itemHD.nextOffset;
    }
    
    if(!itemOffset) {
        clearTableTHNoMain(search);
        search = NULL;
    }
    
    return search;
}

void clearTableHash(TableHash *table) {
    KeyspaceHash *ks = NULL;
    KeyspaceHash *prev = NULL;
    Item *item = NULL;
    Item *del = NULL;
    
    if(table) {
        for (int i = 0; i < table->maxSize; i++) {
            ks = table->th[i].ks;
            while (ks) {
                prev = ks;
                item = ks->item;
                while (item) {
                    del = item;
                    free(item->info);
                    item = item->next;
                    free(del);
                }
                free(ks->key);
                ks = ks->next;
                free(prev);
            }
        }
        free(table->th);
        free(table);
    }
}
