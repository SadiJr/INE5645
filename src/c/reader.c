#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "process.h"

int row, column;

void bubble(int shmid, caralho *arr);
void swap(caralho * array, int row, int column);
void treatInput(char* inputMessage, char* errorMessage, int* parameter);

void printArray(int **array) {
    int i, j;
    for(i = 0; i < row; i++) {
        for(j = 0; j < column; j++) {
            printf("%d ", array[i][j]);
        }
        printf("\n");
    }
}

int main()
{
    int shmid;
    key_t key = 1234;

    int processors;

    treatInput("Expected rows: ", "Please type only numbers, idiot!", &row);
    treatInput("Expected columns: ", "Please type only numbers, idiot!", &column);
    treatInput("Expected processors: ", "Please type only numbers, idiot!", &processors);

    caralho *arr;
    if ((shmid = shmget(key, sizeof(arr), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    } 

    void *shared_memory = (void *)0;

    if ((shared_memory = shmat(shmid, 0, 0)) == -1) {
        perror("shmat");
        _exit(1);
    }
    arr = (caralho *)shared_memory;
    arr->array = (int**)malloc(row * sizeof(int*));

    srand(time(NULL));
    for (int i = 0; i < row; i++) 
        arr->array[i] = (int*)malloc(sizeof(int) * column);
    
    pthread_mutex_init(&arr->lock, 0);
    arr->row = 0;
   
    printf("Creating matriz with random numbers\n");
   
    int i, j;
    for (i = 0; i < row; i++) {
        for (j = 0; j < column; j++) {
            int random = rand() % 1000;
            arr->array[i][j] = random;
        }
    }
    printArray(arr->array);

    int row = 0;
    pid_t pid, wpid;
    int status = 0;

    for(i = 0; i < processors; i++) {
        pid_t pid = fork();
        if(pid < 0) {
            printf("Error in creation of process");
            exit(EXIT_FAILURE);
        } else if(pid == 0) {
            printf("\n\nI am the child!\tMy process id: %d\n",getpid());
            bubble(shmid, arr);
            printf("Finish child %d\n", getpid());
            exit(0);
        }
    }
    while ((wpid = wait(&status)) == 0);

    sleep(5);
    printf("\n\nWakeup\n\n");
    if ((shared_memory = shmat(shmid, 0, 0)) == -1) {
        perror("shmat");
        _exit(1);
    }
    arr = (caralho *)shared_memory;
    printf("Last row %d\n", arr->row);

    printArray(arr->array);

    if (shmdt(arr) == -1) {
         perror("shmdt");
         _exit(1);
    }
    /* Delete the shared memory segment. */
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        _exit(1);
    }
    return 0;
}

void swap(caralho * array, int row, int column) {
    int temp = array->array[row][column];
    array->array[row][column] = array->array[row][column + 1];
    array->array[row][column + 1] = temp;
}

/*
void bubble(int r, int **array) {
    if(r >= row) {
        exit(EXIT_SUCCESS);
    }

    printf("Fork array is:\n");
    printArray(array);

    for (int i = 0; i < column; i++) {
        for (int j = 0; j < column - 1; j++) {
            if (array[r][j] > array[r][j + 1]) {
                swap(array, r, j);
            }
        }
    }
    printf("\nOrdened array row %d is:\n\n", r);
    printArray(array);
}
*/

void bubble(int shmid, caralho *arr) {
    int running = 1;

    // int shmid;
    // caralho *arr;
    // if ((shmid = shmget((key_t)1234, sizeof(arr), IPC_CREAT | 0666)) < 0) {
    //     perror("shmget");
    //     exit(1);
    // } 
    // void *shared_memory = (void *)0;

    // if ((shared_memory = shmat(shmid, 0, 0)) == -1) {
    //     perror("shmat");    
    // }
    
    // printf("Memory attached at %X\n", (int)shared_memory);
    // arr = (caralho *)shared_memory;
    
    while (running) {
        pthread_mutex_lock(&arr->lock);
        int r = arr->row;
        arr->row++;
        pthread_mutex_unlock(&arr->lock);

        if(r >= row) {
            running = 0;
            continue;
        }
        int **temp = arr->array;

        for (int i = 0; i < column; i++) {
            for (int j = 0; j < column - 1; j++) {
                if (temp[r][j] > temp[r][j + 1]) {
                    swap(arr, r, j);
                }
            }
        }
        pthread_mutex_lock(&arr->lock);
        printf("\nOrdened array by process %d in row %d is:\n\n", getpid(), r);
        printArray(temp);
        pthread_mutex_unlock(&arr->lock);
        sleep(1);
    }
    if ((shmdt(arr)) == -1) {
      perror("shmdt");
      return 1;
   }

   printf("Writing Process: Complete\n");
   return 0;

}


void treatInput(char* inputMessage, char* errorMessage, int* parameter)
{
    int expectedInput = 0;
    do {
        printf(inputMessage);
        if (scanf("%d", parameter) == 1) {
            expectedInput = 1;
        }
        else {
            printf(errorMessage);
            fgetc(stdin);
            continue;
        }
    } while (!expectedInput);
}