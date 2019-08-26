#include <pthread.h>

typedef struct caralho {
    pthread_mutex_t lock;
    int row;
    int **array;
} caralho;