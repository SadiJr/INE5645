#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "process.h"

// Methods
void merge_sort(int l, int r);
void merge_sort_sub_process();
void bubble_sort_sub_process();
void initialize_mutex();
void bubble();

// Atributes
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

key_t controll = 77;
key_t matrix_key = 90;

int shmid_controll;
int shmid_matrix;

struct controll *ctrl;
void *shared_memory_controll;

int *matrix;
void *shared_memory_matrix;
clock_t start;
clock_t end;
double total;
int processors;

void read_matrix() {
  shared_memory_matrix = (void *)0;

  if ((shmid_matrix =
           shmget(matrix_key, sizeof(int) * (ctrl->rowSize * ctrl->columnSize),
                  0666 | IPC_CREAT)) < 0) {
    perror("shmget in matrix");
    exit(1);
  }

  if ((shared_memory_matrix = shmat(shmid_matrix, NULL, 0)) == -1) {
    perror("shmat  in matrix");
    _exit(1);
  }
  // printf("\nMatrix Memory attached at %X\n", (int)shared_memory_matrix);

  matrix = (int *)shared_memory_matrix;

  if (matrix[0] == NULL) {
    printf("Error! The matrix in shared memory is empty\n");
    return;
  }
}

void read_controll() {
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
  printf("Controll Memory attached at %X", (int)shared_memory_controll);

  ctrl = (struct controll *)shared_memory_controll;
  print_details(ctrl);
}

int main() {
  read_controll();

  while (ctrl->populated == 0) {
    printf("Waiting another program write data in matriz...\n");
    fflush(stdin);
    sleep(1);
  }

  initialize_mutex();
  bubble_sort_sub_process();

  while (ctrl->populated == 0);
  merge_sort_sub_process();
  pthread_mutex_destroy(&ctrl->mutex);
  ctrl->finished = 1;
  return 0;
}

void bubble_sort_sub_process() {
  treatInput("\nExpected number of processors: ", ERROR_MESSAGE, &processors);
  pid_t pid, wpid;
  int i, status = 0;

  start = clock();
  for (i = 0; i < processors; i++) {
    pid = fork();
    if (pid < 0) {
      printf("Error in creation of process");
      exit(EXIT_FAILURE);
    } else if (pid == 0) {
      // printf("\n\nI am the child!\tMy process id: %d\n", getpid());
      bubble();
      // printf("Finish child %d\n", getpid());
      exit(0);
    }
  }
  while ((wpid = wait(&status)) == 0);

  end = clock();
  total = ((double)(end - start)) / CLOCKS_PER_SEC;
  // FIXME Change messages!
  printf("\n\n\nFinish Bubble Sort algorithm. Total time to proccess array is %f seconds "
         "and the result is:\n\n\n",
         total);

  ctrl->populated = 0;
}

void initialize_mutex() {
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&ctrl->mutex, &attr);
}

void swap(int index) {
  int temp = matrix[index];
  matrix[index] = matrix[index + 1];
  matrix[index + 1] = temp;
}

void bubble() {
  read_matrix();
  int running = 1;

  while (running) {
    pthread_mutex_lock(&ctrl->mutex);
    int r = ctrl->row;
    // printf("\nCurrent process %d working in row  %d\n", getpid(), r);
    ctrl->row++;
    pthread_mutex_unlock(&ctrl->mutex);

    if (r >= ctrl->rowSize) {
      running = 0;
      continue;
    }

    int i, j;
    for (i = 0; i < ctrl->columnSize; i++) {
      for (j = 0; j < ctrl->columnSize - 1; j++) {
        if (matrix[i * ctrl->columnSize + j] >
            matrix[i * ctrl->columnSize + j + 1]) {
          swap(i * ctrl->columnSize + j);
        }
      }
    }
    pthread_mutex_lock(&ctrl->mutex);
    // print_array(matrix, ctrl->rowSize, ctrl->columnSize);
    pthread_mutex_unlock(&ctrl->mutex);
  }
}

/*  Because C is not capable of having 2 identically named functions
    which take different parameters.
    Resumming, no overloading. :(
*/
void init_merge() {

  read_matrix();
  int running = 1;

  while (running) {
    pthread_mutex_lock(&ctrl->mutex);
    int r = ctrl->row;
    // printf("\nCurrent process %d working in row %d\n", getpid(), r);
    ctrl->row++;
    pthread_mutex_unlock(&ctrl->mutex);

    if (r >= ctrl->rowSize) {
      running = 0;
      continue;
    }

    merge_sort(r * ctrl->columnSize,
               r * ctrl->columnSize + ctrl->columnSize - 1);
    pthread_mutex_lock(&ctrl->mutex);
    // print_array(matrix, ctrl->rowSize, ctrl->columnSize);
    pthread_mutex_unlock(&ctrl->mutex);
  }
}

void merge(int l, int m, int r) {
  int i, j, k;
  int n1 = m - l + 1;
  int n2 = r - m;

  /* create temp arrays */
  int L[n1], R[n2];

  /* Copy data to temp arrays L[] and R[] */
  for (i = 0; i < n1; i++)
    L[i] = matrix[l + i];
  for (j = 0; j < n2; j++)
    R[j] = matrix[m + 1 + j];

  /* Merge the temp arrays back into arr[l..r]*/
  i = 0; // Initial index of first subarray
  j = 0; // Initial index of second subarray
  k = l; // Initial index of merged subarray
  while (i < n1 && j < n2) {
    if (L[i] <= R[j]) {
      matrix[k] = L[i];
      i++;
    } else {
      matrix[k] = R[j];
      j++;
    }
    k++;
  }

  /* Copy the remaining elements of L[], if there
     are any */
  while (i < n1) {
    matrix[k] = L[i];
    i++;
    k++;
  }

  /* Copy the remaining elements of R[], if there
     are any */
  while (j < n2) {
    matrix[k] = R[j];
    j++;
    k++;
  }
}

void merge_sort(int l, int r) {
  if (l < r) {
    // Same as (l+r)/2, but avoids overflow for
    // large l and h
    int m = l + (r - l) / 2;

    // Sort first and second halves
    merge_sort(l, m);
    merge_sort(m + 1, r);

    merge(l, m, r);
  }
}

void merge_sort_sub_process() {
  pid_t pid, wpid;
  int i, status = 0;

  start = clock();
  for (i = 0; i < processors; i++) {
    pid = fork();
    if (pid < 0) {
      printf("Error in creation of process");
      exit(EXIT_FAILURE);
    } else if (pid == 0) {
      // printf("\n\nI am the child!\tMy process id: %d\n", getpid());
      init_merge();
      // printf("Finish child %d\n", getpid());
      exit(0);
    }
  }
  while ((wpid = wait(&status)) == 0)
    ;

  end = clock();
  total = ((double)(end - start)) / CLOCKS_PER_SEC;
  printf("\n\n\nFinish Merge Sort algorithm. Total time to proccess array is %f "
         "seconds\n\n\n",
         total);
}