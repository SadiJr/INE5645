#include<stdio.h>
#include<stdlib.h> 
#include<pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include"../resources/log.h"

#define FILE_NAME "../logs/log.log"

int main(int argc, char **argv) {
    if(argv[1] == NULL || argv[2] == NULL) {
        log_error("Wrong parameters, idiot!");
        return 1;
    }
    struct stat st = {0};

    if (stat("../logs", &st) == -1) {
        mkdir("../logs", 0700);
    }

    FILE *file = fopen(FILE_NAME, "ab+");
    log_set_fp(file);
    
    int size = strtol(argv[1], NULL, 10);
    int numbers = strtol(argv[2], NULL, 10);

    log_info("Size of array is %i", size);
    log_info("Numbers of arrays is %i", numbers);

    log_info("Creating random multidimensional array...");

    
    int a[numbers][size];

    for(int i = 0; i < numbers; i++) {
        for(int j = 0; j < size; j++) {
            a[i][j] = rand();
        }
    }

    log_info("Random Multidimensional array created!");
    log_info("Starting the ordenation algorithms...");
    return 0;
}