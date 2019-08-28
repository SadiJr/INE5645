#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

#define FALSE 0
#define TRUE 1

void treatInput(char* inputMessage, char* errorMessage, int* parameter);
void createThreads(int threadsNumber, int bubble);
void* bubble_sort(void* args);
void swap(int row, int column);
void printArray();
void* init_merge_sort(void* args);

int** arr;
int columnSize;
int rowsNumber;
int threadsNumber;

int main(int argc, char** argv)
{
    long number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);

    int row, column, threads;
    treatInput("Expected numbers of rows: ", "Please type only numbers, idiot!\n", &row);
    treatInput("Expected size of columns: ", "Please type only numbers, idiot!\n", &column);
    treatInput("Expected number of threads: ", "Please type only numbers, idiot!\n", &threads);

    if (threads > number_of_processors) {
        printf("The max numbers of processors in system (%ld) is letter then your "
               "number of threads!\nChanging the number of threads to max number of "
               "processors in this system...\n",
            number_of_processors);
        threads = number_of_processors;
    }
    threadsNumber = threads;
    columnSize = column;
    rowsNumber = row;

    arr = (int**)malloc(row * sizeof(int*));
    int** arr2 = (int**)malloc(row * sizeof(int*));

    srand(time(NULL));
    for (int i = 0; i < row; i++) {
        arr[i] = (int*)malloc(sizeof(int) * column);
        arr2[i] = (int*)malloc(sizeof(int) * column);
    }
    printf("Creating matriz with random numbers\n");
    int i, j;
    for (i = 0; i < row; i++) {
        for (j = 0; j < column; j++) {
            int random = rand() % 1000;
            arr[i][j] = random;
            arr2[i][j] = random;
        }
    }
    printf("The generated array is:\n\n");
    printArray();
    printf("\n\n");

    printf("Bubble Sort Algorithm: ");
    createThreads(threads, TRUE);

    printf("\n\n");
    printf("Restore original array...");
    arr = arr2;
    printf("Restored array is:\n\n");
    printArray();
    printf("\n\n");

    printf("Merge Sort Algorithm: ");
    createThreads(threads, FALSE);

    free(arr);
    pthread_exit(NULL);

    return TRUE;
}

void treatInput(char* inputMessage, char* errorMessage, int* parameter)
{
    int expectedInput = FALSE;
    do {
        printf(inputMessage);
        if (scanf("%d", parameter) == 1) {
            expectedInput = TRUE;
        }
        else {
            printf(errorMessage);
            fgetc(stdin);
            continue;
        }
    } while (!expectedInput);
}

void createThreads(int threadsNumber, int bubble)
{
    pthread_t threads[threadsNumber];
    double total;
    clock_t start;

    start = clock();
    for (int i = 0; i < threadsNumber; i++) {
        printf("Creating thread %i\n", i);
        if (bubble) {
            pthread_create(&threads[i], NULL, bubble_sort, (void*)(intptr_t)i);
        }
        else {
            start = clock();
            pthread_create(&threads[i], NULL, init_merge_sort, (void*)(intptr_t)i);
        }
    }

    int rc;
    for (int i = 0; i < threadsNumber; i++) {
        rc = pthread_join(threads[i], NULL);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    clock_t end = clock();
    total = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\nFinish algorithm. Total time to proccess array is %f seconds and the result is:\n\n\n", total);
    printArray();
}

void* bubble_sort(void* args)
{
    int running = TRUE;
    int row = (intptr_t)args;

    while (running) {
        if (row >= rowsNumber) {
            running = FALSE;
            continue;
        }

        printf("\nPthread %lu processing row number %i", pthread_self(), row);
        for (int i = 0; i < columnSize; i++) {
            for (int j = 0; j < columnSize - 1; j++) {
                if (arr[row][j] > arr[row][j + 1]) {
                    swap(row, j);
                }
            }
        }
        row += threadsNumber;
    }
    pthread_exit((void*)args);
}

void swap(int row, int column)
{
    int temp = arr[row][column];
    arr[row][column] = arr[row][column + 1];
    arr[row][column + 1] = temp;
}

void printArray()
{
    for (int i = 0; i < rowsNumber; i++) {
        for (int j = 0; j < columnSize; j++) {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
}

void merge(int row, int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    /* create temp arrays */
    int L[n1], R[n2];

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[row][l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[row][m + 1 + j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[row][k] = L[i];
            i++;
        }
        else {
            arr[row][k] = R[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there 
     are any */
    while (i < n1) {
        arr[row][k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there 
     are any */
    while (j < n2) {
        arr[row][k] = R[j];
        j++;
        k++;
    }
}

/* l is for left index and r is right index of the 
   sub-array of arr to be sorted */
void mergeSort(int row, int l, int r)
{
    if (l < r) {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l + (r - l) / 2;

        // Sort first and second halves
        mergeSort(row, l, m);
        mergeSort(row, m + 1, r);

        merge(row, l, m, r);
    }
}

void* init_merge_sort(void* args)
{
    int row = (intptr_t)args;
    int arr_size = columnSize;
    int running = TRUE;

    while (running) {
        if (row >= rowsNumber) {
            running = FALSE;
            continue;
        }

        printf("\nPthread %lu processing row number %i", pthread_self(), row);
        mergeSort(row, 0, arr_size - 1);
        row += threadsNumber;
    }
    pthread_exit((void*)args);
}