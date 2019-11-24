#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

double parallel, sequential;

unsigned int get_max(int *array, int size) {
  unsigned int max = array[0];

  for (int i = 1; i < size; i++) {
    if (array[i] > max) max = array[i];
  }
  return max;
}

unsigned int get_min(int *array, int size) {
  unsigned int min = array[0];

  for (int i = 1; i < size; i++) {
    if (array[i] < min) min = array[i];
  }
  return min;
}

int compare_ints(const void *arg1, const void *arg2) {
  int a1 = *(int *)arg1;
  int a2 = *(int *)arg2;
  if (a1 < a2)
    return -1;
  else if (a1 == a2)
    return 0;
  else
    return 1;
}

void quick_sort(int *list, int end) {
  qsort(list, (size_t)end, sizeof(int), compare_ints);
}

void print_array(int *list, int size) {
  int i;
  for (i = 0; i < size; i++) printf("%i ", list[i]);
  printf("\n\n");
}

void parallel_sort(int *parallel_array, int size, int numprocs, MPI_Status status) {
  int i, j;

  double start_time, end_time;
  start_time = MPI_Wtime();

  int *short_bucket[numprocs];
  int bucket_sizes[numprocs];
  int bucket_sizes_temp[numprocs];

  unsigned int max = get_max(parallel_array, size);
  unsigned int min = get_min(parallel_array, size);

  for (i = 0; i < numprocs; i++) {
    bucket_sizes[i] = 0;
    bucket_sizes_temp[i] = 0;
  }

  double pivot = (((double)max - min + 1) / (numprocs));
  for(i = 0; i < size; i++) {
    int idx = (parallel_array[i] - min) / pivot;
    bucket_sizes[idx]++;
    bucket_sizes_temp[idx]++;
  }

  for (i = 0; i < numprocs; i++) {
    short_bucket[i] = (int *)malloc(sizeof(int) * bucket_sizes[i]);
  }

  /* Partitionate the global array into the bukets*/
  for (i = 0; i < size; i++) {
    int idx = (parallel_array[i] - min) / pivot;
    bucket_sizes_temp[idx]--;
    short_bucket[idx][bucket_sizes_temp[idx]] = parallel_array[i];
  }

  /*Send buckets to each process*/
  for (i = 1; i < numprocs; i++) {
    MPI_Send(&bucket_sizes[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    MPI_Send(short_bucket[i], bucket_sizes[i], MPI_INT, i, 0, MPI_COMM_WORLD);
  }

  /*Sort bucket 0*/
  quick_sort(short_bucket[0], bucket_sizes[0]);

  /*Receive Sorted buckets from each process*/
  for (i = 1; i < numprocs; i++) {
    MPI_Recv(short_bucket[i], bucket_sizes[i], MPI_INT, i, 0, MPI_COMM_WORLD, &status);
  }

  /*Concatenate sorted buckets*/
  int index = 0;
  for (i = 0; i < numprocs; i++) {
    for (j = 0; j < bucket_sizes[i]; j++) {
      parallel_array[index++] = short_bucket[i][j];
    }
  }

  end_time = MPI_Wtime();
  // print_array(parallel_array, size);
  parallel = end_time - start_time;
  printf("Total parallel processing time using %d processes operating on a array of size %d: %f seconds\n\n", numprocs, size, parallel);

  free(parallel_array);
  free(*short_bucket);
}

void sequential_sort(int *sequential_array, int size, int buckets) {
  int i, j, index;

  clock_t t;
  t = clock();

  int *short_bucket[buckets];
  int bucket_sizes[buckets];
  int bucket_sizes_temp[buckets];

  unsigned int max = get_max(sequential_array, size);
  unsigned int min = get_min(sequential_array, size);

  for (i = 0; i < buckets; i++) {
    bucket_sizes[i] = 0;
    bucket_sizes_temp[i] = 0;
  }

  /* Partitionate the global array into the bukets*/
  double pivot = (((double)max - min + 1) / (buckets));
  for (i = 0; i < size; i++) {
    int index = (sequential_array[i] - min) / pivot;
    bucket_sizes_temp[index]++;
    bucket_sizes[index]++;
  }

  for (i = 0; i < buckets; i++) {
    short_bucket[i] = (int *)malloc(sizeof(int) * bucket_sizes[i]);
  }

  for (i = 0; i < size; i++) {
    int index = (sequential_array[i] - min) / pivot;
    bucket_sizes_temp[index]--;
    short_bucket[index][bucket_sizes_temp[index]] = sequential_array[i];
  }

  j = 0;
  for (i = 0; i < buckets; i++) {
    quick_sort(short_bucket[i], bucket_sizes[i]);
  }

  index = 0;
  for (i = 0; i < buckets; i++) {
    for (j = 0; j < bucket_sizes[i]; j++) {
      sequential_array[index++] = short_bucket[i][j];
    }
  }
  t = clock() - t;
  sequential = ((double)t) / CLOCKS_PER_SEC;
    
  // print_array(sequential_array, size);
  printf("Total sequential processing time using %d buckets operating on a array of size %d: %f seconds\n\n", buckets, size, sequential);

  free(sequential_array);
  free(*short_bucket);
}

int main(int argc, char **argv) {
  int numprocs, rank, i, j, size, rc;

  rc = MPI_Init(&argc, &argv);
  if (rc != MPI_SUCCESS) {
    printf("Error starting MPI program. Terminating.\n");
    MPI_Abort(MPI_COMM_WORLD, rc);
  }

  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (argc != 2) {
    if (rank == 0) printf("Please input the size of array to process!\n");
    return EXIT_FAILURE;
  }

  size = atoi(argv[1]);

  if (size <= 0) {
    if (rank == 0) printf("The size must be > 0, idiot!.\n");
    return EXIT_FAILURE;
  }

  if (rank == 0) {
    MPI_Status status;

    int *parallel_array = NULL;
    int *sequential_array = NULL;

    printf("Number of slayers (in parallel process) and buckets (in sequential process): %d\nArray size: %d\n", numprocs, size);

    parallel_array = (int *)malloc(sizeof(int) * size);
    sequential_array = (int *)malloc(sizeof(int) * size);

    /* Create an array of size n and fill it with random numbers*/
    // printf("Creating arrays...\n");
    
    srand(time(NULL));
    for (i = 0; i < size; i++) {
      int random = rand() / 1000;
      parallel_array[i] = random;
      sequential_array[i] = random;
    }

    // printf("Array created is:\n");
    // print_array(parallel_array, size);

    printf("Starting parallel processing...\n");
    parallel_sort(parallel_array, size, numprocs, status);

    printf("Starting sequential process...\n");
    // print_array(sequential_array, size);
    sequential_sort(sequential_array, size, numprocs);
    printf("In an environment with %d processes operating over a array of size %d, the speedup was: %f\n\n", numprocs, size, sequential/parallel);
  } else {
    int size;
    MPI_Status status;

    /*Receive the size of bucket*/
    MPI_Recv(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    /*Alloc memory and receive the bucket*/
    int *bucket = (int *)malloc(sizeof(int) * size);
  
    MPI_Recv(bucket, size, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    quick_sort(bucket, size);

    MPI_Send(bucket, size, MPI_INT, 0, 0, MPI_COMM_WORLD);
    free(bucket);
  }
  MPI_Finalize();
  return 0;
}