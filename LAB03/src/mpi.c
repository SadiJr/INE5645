#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int compare_ints(const void* arg1, const void* arg2) {
  int a1 = *(int *)arg1;
  int a2 = *(int *)arg2;
  if (a1 < a2 ) return -1;
  else if (a1 == a2 ) return 0;
  else return 1;
}

void serialQuicksort(int *list, int end) {
	qsort(list, (size_t)end, sizeof(int), compare_ints);
}

void print_array(int *list, int n) {
  int i;
  for (i = 0; i < n; i++) 
  	printf("%i ", list[i]);
  printf("\n\n");
}

int main(int argc, char **argv) {
  int numprocs, rank, i, j, n, rc;

  rc = MPI_Init(&argc, &argv);
  if (rc != MPI_SUCCESS) {
    printf("Error starting MPI program. Terminating.\n");
    MPI_Abort(MPI_COMM_WORLD, rc);
  }

  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (argc != 2) {
    if (rank == 0) printf("Need one argument (= the list size n).\n");
    return EXIT_FAILURE;
  }

  n = atoi(argv[1]);

  if (n <= 0) {
    if (rank == 0) printf("List size must be > 0.\n");
    return EXIT_FAILURE;
  }

  if (rank == 0) {
    MPI_Status status;

    int *globalArray = NULL;
    int *serialArray = NULL;

    printf("\nNumber of slayers: %d\nArray size: %d\n", numprocs, n);

    int *short_bucket[numprocs];
    int bucket_sizes[numprocs];

    srand(time(NULL));

    globalArray = (int *)malloc(sizeof(int) * n);
    serialArray = (int *)malloc(sizeof(int) * n);

    unsigned int max = 0;
    unsigned int min = 0;

    /* Create an array of size n and fill it with random numbers*/
    for (i = 0; i < n; i++) {
      int random = rand() / 1000;
      globalArray[i] = random;
      serialArray[i] = random;
      
    }
    
	double start_time, end_time;
    start_time = MPI_Wtime();

	for (i = 0; i < n; i++) {
		if (globalArray[i] > max)
        	max = globalArray[i];

      	if (globalArray[i] < min)
      	  min = globalArray[i];
	}

    /*Alloc memory for buckets and initialize the size on 0*/
    for (i = 0; i < numprocs; i++) {
      short_bucket[i] = (int *)malloc(sizeof(int) * n);
      bucket_sizes[i] = 0;
    }


    /* Partitionate the global array into the bukets*/
    double pivot = (((double)max - min + 1) / (numprocs));
    for (i = 0; i < n; i++) {
      int index = (globalArray[i] - min) / pivot;
      short_bucket[index][bucket_sizes[index]] = globalArray[i];
      bucket_sizes[index]++;
    }

	for (i = 0; i < numprocs; i++) {
	      short_bucket[i] = (int *)malloc(sizeof(int) * bucket_sizes[i]);
	}

    /*Send buckets to each process*/
    for (i = 1; i < numprocs; i++) {
      MPI_Send(&bucket_sizes[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      MPI_Send(short_bucket[i], bucket_sizes[i], MPI_INT, i, 0, MPI_COMM_WORLD);
    }

    /*Sort bucket 0*/
    serialQuicksort(short_bucket[0], bucket_sizes[0]);

    /*Receive Sorted buckets from each process*/
    for (i = 1; i < numprocs; i++) {
      MPI_Recv(short_bucket[i], bucket_sizes[i], MPI_INT, i, 0, MPI_COMM_WORLD,
               &status);
    }

    /*Concatenate sorted buckets*/
    int index = 0;
    for (i = 0; i < numprocs; i++) {
      for (j = 0; j < bucket_sizes[i]; j++) {
        globalArray[index++] = short_bucket[i][j];
      }
    }
    end_time = MPI_Wtime();
    printf("\n\nTotal time: %f secs\n", end_time - start_time);

	clock_t t;
    t = clock();

	serialQuicksort(serialArray, n);
	t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds

    printf("\n\nProgram took %f seconds to execute \n", time_taken);
  } else {
    int size;

    MPI_Status status;

    /*Receive the size of bucket*/
    MPI_Recv(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    /*Alloc memory and receive the bucket*/
    int *bucket = (int *)malloc(sizeof(int) * size);

    MPI_Recv(bucket, size, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    serialQuicksort(bucket, size);

    MPI_Send(bucket, size, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}
