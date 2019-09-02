#include <stdio.h>
#include <pthread.h>
#define ERROR_MESSAGE "Please type only numbers, idiot!\n"

struct controll {
  pthread_mutex_t mutex;
  int row;
  int rowSize;
  int columnSize;
  int populated;
  int finished;
};

void print_details(struct controll *ctrl) {
  printf("\nCurrent row %d\n", ctrl->row);
  printf("Row size %d\n", ctrl->rowSize);
  printf("Column size %d\n", ctrl->columnSize);
  printf("Is populated? %d\n", ctrl->populated);
  printf("Is finished? %d\n", ctrl->finished);
}

void print_array(int *array, int row, int column) {
  int i, j;
  for (i = 0; i < row; i++) {
    for (j = 0; j < column; j++) {
      printf("%d ", array[i * column + j]);
    }
    printf("\n");
  }
}

void treatInput(char *inputMessage, char *errorMessage, int *parameter) {
  int expectedInput = 0;
  do {
    printf(inputMessage);
    //Comment out lines 37, 41 and 42 if you are going to run the application without logging
    fflush(stdout);
    if (scanf("%d", parameter) == 1) {
      expectedInput = 1;
      printf("%d", *parameter);
      fflush(stdout);
    } else {
      printf(errorMessage);
      fgetc(stdin);
      continue;
    }
  } while (!expectedInput);
}