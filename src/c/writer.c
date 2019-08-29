#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "process.h"

key_t controll = 77;
key_t matrix_key = 90;

int shmid_controll;
int shmid_matrix;

struct controll *ctlr;
void *shared_memory_controll;

int *matrix;
int *temp;
void *shared_memory_matrix;

int row, column;

void finish_shared_memory() {
  if (shmdt(((struct controll *)shared_memory_controll)) == -1) {
    perror("shmdt in controll");
    _exit(1);
  }

  /* Delete the shared memory segment. */
  if (shmctl(shmid_controll, IPC_RMID, NULL) == -1) {
    perror("shmctl in controll");
    _exit(1);
  }

  if (shmdt(shared_memory_matrix) == -1) {
    perror("shmdt in matrix");
    _exit(1);
  }

  /* Delete the shared memory segment. */
  if (shmctl(shmid_matrix, IPC_RMID, NULL) == -1) {
    perror("shmctl in matrix");
    _exit(1);
  }

  free(temp);
}

void create_random_matrix() {
  srand(time(NULL));
  printf("Creating matriz with random numbers...\nCreated matrix is:\n");

  int i;
  for (i = 0; i < row * column; i++) {
    int random = rand() % 1000;
    matrix[i] = random;
    temp[i] = random;
  }

  ctlr->populated = 1;
  print_array(matrix, row, column);
}

void create_matrix() {
  shared_memory_matrix = (void *)0;

  treatInput("\nExpected number of rows: ", ERROR_MESSAGE, &row);
  treatInput("Expected number of columns: ", ERROR_MESSAGE, &column);
  ctlr->rowSize = row;
  ctlr->columnSize = column;

  temp = malloc((row * column) * sizeof(int));
  if ((shmid_matrix = shmget(matrix_key, (row * column) * sizeof(int),
                             0666 | IPC_CREAT)) < 0) {
    perror("shmget in matrix");
    exit(1);
  }

  if ((shared_memory_matrix = shmat(shmid_matrix, NULL, 0)) == -1) {
    perror("shmat  in matrix");
    _exit(1);
  }
  printf("\nMatrix Memory attached at %X\n", (int)shared_memory_matrix);

  matrix = (int *)shared_memory_matrix;

  if (matrix == NULL) {
    printf("Error! The matrix in shared memory is empty\n");
    if (shmdt(shared_memory_matrix) == -1) {
      perror("shmdt in matrix");
      _exit(1);
    }

    /* Delete the shared memory segment. */
    if (shmctl(shmid_matrix, IPC_RMID, NULL) == -1) {
      perror("shmctl in matrix");
      _exit(1);
    }
    return;
  }
  create_random_matrix();
}

void create_controll_shared_memory() {
  shared_memory_controll = (void *)0;

  if ((shmid_controll =
           shmget(controll, sizeof(struct controll *), 0666 | IPC_CREAT)) < 0) {
    perror("shmget in controll");
    exit(1);
  }

  if ((shared_memory_controll = shmat(shmid_controll, NULL, 0)) == -1) {
    perror("shmat in controll");
    _exit(1);
  }
  printf("Controll Memory attached at %X\n", (int)shared_memory_controll);

  ctlr = (struct controll *)shared_memory_controll;
}

int main() {
  create_controll_shared_memory();
  create_matrix();

  while (ctlr->finished == 0) {
    if (ctlr->populated == 0) {
      printf("\nOrdened matrix is:\n");
      print_array(matrix, ctlr->rowSize, ctlr->columnSize);
      printf("\nRestoring original matrix...\n");
      int i;
      for (i = 0; i < ctlr->rowSize * ctlr->columnSize; i++) {
        matrix[i] = temp[i];
      }
      printf("Restored matrix is:\n");
      print_array(matrix, ctlr->rowSize, ctlr->columnSize);
      ctlr->row = 0;
      ctlr->populated = 1;
    }
    sleep(1);
  }

  printf("\n\nOrdened matrix is:\n");
  print_array(matrix, ctlr->rowSize, ctlr->columnSize);
  print_details(ctlr);
  printf("\nFinishing shared memory...\n");
  finish_shared_memory();
  printf("Process finished. Thanks a lot!");
  return 0;
}