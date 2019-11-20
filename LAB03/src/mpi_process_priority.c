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

  unsigned int max = get_max(parallel_array, size);
  unsigned int min = get_min(parallel_array, size);

  // printf("\n\nMax and min for native function is %d and %d\n\n", max, min);

  /*Alloc memory for buckets and initialize the size on 0*/
  for (i = 0; i < numprocs; i++) {
    short_bucket[i] = (int *)malloc(sizeof(int) * size);
    bucket_sizes[i] = 0;
  }

  /* Partitionate the global array into the bukets*/
  double pivot = (((double)max - min + 1) / (numprocs));
  for (i = 0; i < size; i++) {
    int index = (parallel_array[i] - min) / pivot;
    short_bucket[index][bucket_sizes[index]] = parallel_array[i];
    bucket_sizes[index]++;
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
  printf("Total time in parallel process: %f seconds\n\n", parallel);
}

void sequential_sort(int *sequential_array, int size, int buckets) {
  clock_t t;
  t = clock();


  int i, j, index;

  int *short_bucket[buckets];
  int bucket_sizes[buckets];

  unsigned int max = get_max(sequential_array, size);
  unsigned int min = get_min(sequential_array, size);

  for (i = 0; i < buckets; i++) {
    short_bucket[i] = (int *)malloc(sizeof(int) * size);
    bucket_sizes[i] = 0;
  }

  /* Partitionate the global array into the bukets*/
  double pivot = (((double)max - min + 1) / (buckets));
  for (i = 0; i < size; i++) {
    int index = (sequential_array[i] - min) / pivot;
    short_bucket[index][bucket_sizes[index]] = sequential_array[i];
    bucket_sizes[index]++;
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
  printf("Total time in sequential process: %f seconds\n", sequential);
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

    printf("\nNumber of slayers (in parallel process) and buckets (in sequential process): %d\nArray size: %d\n", numprocs, size);

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

    printf("\nStarting parallel processing...\n");
    parallel_sort(parallel_array, size, numprocs, status);

    printf("Starting sequential process...\n");
    // print_array(sequential_array, size);

    sequential_sort(sequential_array, size, numprocs);

    printf("Speedup is %f", sequential/parallel);
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
  }
  MPI_Finalize();
  return 0;
}