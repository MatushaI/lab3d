#ifndef TableHashHD_h
#define TableHashHD_h

#include <stdio.h>

typedef struct ItemHHD {
    unsigned long long nextOffset;
    unsigned long long infoOffset;
    unsigned int lenghtInfo;
    unsigned int version;
} ItemHHD;

typedef struct KeySpaceHHD {
    unsigned long long ItemOffset;
    unsigned long long nextOffset;
    unsigned long long keyOffset;
    unsigned int keyLenght;
} KeySpaceHHD;

typedef struct TitleHashHD {
    unsigned long long KeySpaceOffset;
} TitleHashHD;

typedef struct TableHHD {
    unsigned long long allOffset;
    unsigned int sizeTitle;
    FILE *file;
} TableHHD;

// =======(Для поиска)=======

typedef struct Item {
    char *info;
    unsigned int version;
    struct Item *next;
} Item;

typedef struct KeySpaceHash {
    char* key;
    Item *item;
    struct KeySpaceHash *next;
} KeyspaceHash;

typedef struct TitleHash {
    KeyspaceHash *ks;
} TitleHash;

typedef struct TableHash {
    unsigned int maxSize;
    TitleHash *th;
} TableHash;



TableHHD *importTableHHD(FILE *file);
int exportTableHHD(TableHHD *table);
int addInfoHHD(TableHHD *table, char *info, char *key);
void printTableHHD(TableHHD *table);
int deleteKeyHHD(TableHHD *table, char *key);
int deleteOldVersionsHHD(TableHHD *table, char *key);

// =======(Для поиска)=======

TableHash *createTH(int size);
int addInfoTH(TableHash *table, char *key, char *info);
int printTH(TableHash *table);
TableHash *searchKeyTH(TableHHD *table, TableHash *search, char *key);
TableHash *searchKeyVersionTH(TableHHD *table, TableHash *search, char *key, int version);
void clearTableHash(TableHash *table);

#endif 
