#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

#include "../resources/log.h"

#define FILE_NAME "../logs/log.log"
#define FALSE 0
#define TRUE 1

void treatInput(char* inputMessage, char* errorMessage, int *parameter);
void createThreads(int threadsNumber);
void *bubbleSort(void *args);
// void swap(int *x, int *y);
void swap(int row, int column);
void printArray();

int **arr;
int columnSize;
int rowsNumber;
int threadsNumber;

int main(int argc, char **argv) {
	long number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);
	//printf("This system has %d processors configured and %d processors available.\n", get_nprocs_conf(), get_nprocs());
	//int phisicalProcessors = get_nproc
	//if(argc != 3) {
	//	printf("Wrong number of parameters, idiot!\n");
	//	exit(ERROR);
	//}	
	//int expectedInput;

	int row, column, threads;
	treatInput("Expected numbers of rows: ", "Please type only numbers, idiot!\n", &row);
	treatInput("Expected size of columns: ", "Please type only numbers, idiot!\n", &column);
	treatInput("Expected number of threads: ", "Please type only numbers, idiot!\n", &threads);

	if(threads > number_of_processors) {
		printf("The max numbers of processors in system (%ld) is letter then your "
		"number of threads!\nChanging the number of threads to max number of "
		"processors in this system...\n", number_of_processors);
		threads = number_of_processors;
	}
	threadsNumber = threads;
	columnSize = column;
	rowsNumber = row;

	/*
	do {
		printf("Expected numbers of rows: ");
		expectedInput = ERROR;
		
		if(scanf("%d", &row) == 1) {
			expectedInput = SUCCESS;
		} else {
			printf("Please digit only numbers!");
		}
	} while(expectedInput);
	*/
	//int row = atoi(argv[1]);
	//int column = atoi(argv[2]);
	
	printf("Row %i\n", row);
	printf("Column %i\n", column);
	printf("Threads %i\n", threads);

	arr = (int **) malloc(row * sizeof(int *));	
	srand(time(NULL));

	for(int i = 0; i < column; i++) {
		arr[i] = (int*) malloc(sizeof(int) * column);
	}

	printf("Creating matriz with random numbers\n");

	for(int i = 0; i < row; i++) {
		for(int j = 0; j < column; j++) {
			arr[i][j] = rand() % 1000;
		}
	}
	printf("The generated array is:\n\n");
	printArray();
	printf("\n\n");

	createThreads(threads);
	return TRUE;
}

void treatInput(char* inputMessage, char* errorMessage, int *parameter) {
	int expectedInput = FALSE;
	do {
		printf(inputMessage);
		if(scanf("%d", parameter) == 1) {
			expectedInput = TRUE;
		} else {
			printf(errorMessage);
			fgetc(stdin);
			continue;
		}
	} while(!expectedInput);
}

void createThreads(int threadsNumber) {
    pthread_t threads[threadsNumber];

    for (int i = 0; i < threadsNumber; i++) {
		printf("Creating thread %i\n", i);
        pthread_create(&threads[i], NULL, bubbleSort, (void *) (intptr_t)i);
    }

    for (int i = 0; i < threadsNumber; i++) {
        pthread_join(threads[i], NULL);
	}
	printf("\nFinish algorithm. The result is:\n\n\n");
	printArray();

	free(arr);
}

void *bubbleSort(void *args) {
	int running = TRUE;
    int row = (intptr_t)args;

	while(running) {
		if(row >= rowsNumber) {
			running = FALSE;
			continue;
		}

		printf("Processing row number %i\n", row);
		for(int i = 0; i < columnSize; i++) {
    	for (int j = 0; j < columnSize -1; j++) {
        	if (arr[row][j] > arr[row][j+1]) {
        	    swap(row, j);
			}
		}
		}
		row += threadsNumber;
	}
}

void swap(int row, int column) {
	int temp = arr[row][column];
	arr[row][column] = arr[row][column+1];
	arr[row][column + 1] = temp;
}

/*
void swap(int *x, int *y) { 
    int temp = *x; 
    *x = *y; 
    *y = temp; 
}
*/
void printArray() {
	for(int i = 0; i < rowsNumber; i++) {
		for(int j = 0; j < columnSize; j++) {
			printf("%d ", arr[i][j]);
		}
		printf("\n");
	}
}