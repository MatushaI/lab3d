#include "getFunctions.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "TableHashHD.h"

/*
int importFile(TableHD *table, Table *search) {
    int check = 0;
    FILE *file = NULL;
    file = getFile("\nВведите название файла\n", "r", &check);
    char *string = NULL;
    int i = 1;
    int n = 0;
    
    if(file) {
        printf("(OK) Файл найден. Начат импорт\n");
        string = readString(file);
        while (string) {
            if(i % 3 == 1) {
                n = getIntUnsignt(string);
                if(n == -1) {
                    break;;
                }
                free(string);
            }
            if(i % 3 == 2) {
                if(!addInfoHD(table, n, string)) {
                    free(string);
                }
            }
            if(i % 3 == 0) {
                if(strlen(string) == 0) {
                    free(string);
                } else {
                    free(string);
                    break;
                }
            }
            
            i++;
            string = readString(file);
        }
        fclose(file);
        
    } else {
        printf("(X) Файл не найден\n");
    }
    if(check == -1) {
        return 0;
    }
    
    return 1;
}
*/
char *readString(FILE *file) {
    char *ptr = (char *)malloc(1);
    char buf[81];
    int n, len = 0;
    *ptr = '\0';
    do {
        n = fscanf(file, "%80[^\n]", buf);
        if(n < 0) {
            free(ptr);
            ptr = NULL;
            continue;
        }
        if(n == 0) {
            fscanf(file, "%*c", NULL);
        }
        
        else {
            len += strlen(buf);
            ptr = (char *) realloc(ptr, len + 1);
            strcat(ptr, buf);
        }
    } while(n > 0);
        
    return ptr;
}

FILE* getFile(const char* invite, char choose[4], int *check) {
    FILE *file = NULL;
    char *dir = NULL;
    dir = getcwd(dir, 1000);
    dir = strtok(dir, "\t");
    char *s = NULL;
    int lendir = (int) strlen(dir);
    s = myreadline(invite);
    if(s) {
        dir = realloc(dir, lendir*sizeof(char)+strlen(s)*sizeof(char) + 2);
        dir[lendir] = '/';
        dir[lendir+1] = '\0';
        dir = strcat(dir, s);
        file = fopen(dir, choose);
        free(s);
    } else {
        *check = -1;
    }
    printf("%s\n", dir);
    free(dir);
    return file;
}

int getIntUnsignt(char *number) {
    char chars[10] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
    
    if(number != NULL){
        int lenght = (int) strlen(number);
        
        if(number[0] == ' ') {
            return -1;
        }
        
        if(number[0] == '0' && lenght == 1) {
            return 0; 
        }
        
        if(number[0] == '0'){
            return -1;
        }
        
        if(lenght > 9) {
            return -1;
        }
        
        if(lenght == 0) {
            return -1;
        }
        
        int flag = 0;
        
        for(int i = 0; i < lenght; i++) {
            for(int b = 0; b < 10; b++) {
                if(chars[b] == number[i]) {
                    flag++;
                    break;
                }
            }
        }
        
        if(flag == lenght) {
            return atoi(number);
        }
    }
        
    return -1;
}

char *myreadline(const char* invite) {
    {
    char *ptr = (char *)malloc(1);
    char buf[81];
    int n, len = 0;
    *ptr = '\0';
    printf("%s", invite);
    do{
        n = scanf("%80[^\n]", buf);
        if(n < 0){
            free(ptr);
            ptr = NULL;
            continue;
        }
        if(n == 0) {
            scanf("%*c");
        }
        
        else {
            len += strlen(buf);
            ptr = (char *) realloc(ptr, len + 1);
            strcat(ptr, buf);
        }
    } while(n > 0);
    return ptr;
        
    }
}

/*

FILE *importBinaryFile(const char* invite, char choose[4]) {
    FILE *file = NULL;
    int check = 0;
    file = getFile(invite, choose, &check);
    
    if(check == -1) {
        return NULL;
    }
    
    if(file == NULL) {
        printf("\n(X) Файл не найден\n");
    } else {
        return file;
    }
    
    char *bin = ".bin"; 
    int n = 0, N = 2;
    char *errorMessage = "";
    char *string = NULL;
    
    do {
        printf("%s\n", errorMessage);
        printf("Создать таблицу в новом файле?\n0. Нет\n1. Да\n");
        string = myreadline("> ");
        if(string == NULL) {
            return 0;
        }
        n = getIntUnsignt(string);
        errorMessage = "\n(!) Пункт меню не найден. Попробуйте ещё раз\n";
        free(string);
        
    } while (n < 0 || n >= N);
    
    if(n == 0) {
        return NULL;
    } else {
        string = myreadline("\nВведите название файла: ***.bin\n");
        if(string == NULL) {
            return 0;
        }
        int lenght = (int) strlen(string);
        string = realloc(string, lenght + 5);
        for (int i = 0; i < 5; i++) {
            string[lenght + i] = bin[i];
        }
    }
    char *s = string;
    n = 0;
    errorMessage = "";
    
    do {
        printf("%s\n", errorMessage);
        string = myreadline("Введите количество элементов таблицы\n> ");
        
        if(string == NULL) {
            free(s);
            return 0;
        }
        
        n = getIntUnsignt(string);
        errorMessage = "\n(!) Ошибка ввода. Попробуйте ещё раз\n";
        free(string);
        
    } while (n <= 0);
    
    char *dir = NULL;
    dir = getcwd(dir, 1000);
    dir = strtok(dir, "\t");
    int lendir = (int) strlen(dir);
    if(s) {
        dir = realloc(dir, lendir*sizeof(char)+strlen(s)*sizeof(char) + 2);
        dir[lendir] = '/';
        dir[lendir+1] = '\0';
        dir = strcat(dir, s);
        file = fopen(dir, "a+b");
        free(s);
    }
    free(dir);
    
    unsigned long long load = sizeof(unsigned long long) + 2 * sizeof(int) + n * sizeof(KeySpaceHD);
    printf("%llu\n", load);
    fwrite(&load, sizeof(unsigned long long), 1, file);
    load = n;
    fwrite(&load, sizeof(int), 1, file);
    load = 0;
    fwrite(&load, sizeof(int), 1, file);
    
    KeySpaceHD *ks = calloc(n, sizeof(KeySpaceHD));
    
    for(int i = 0; i < n; i++) {
        ks[i].key = 0;
        ks[i].latestVersionOffset = 0;
    }
     
    fwrite(ks, sizeof(KeySpaceHD), n, file);
    free(ks);
    
    return file;
}
*/
