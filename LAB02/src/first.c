#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdint.h>
#include "helper.h"

sem_t mutex;                    /* controla o acesso a 'rc' */
sem_t db;                       /* controla o acesso a base de dados */
int rc = 0;                     /* número de processos lendo ou querendo ler */

char data = ' ';

void* reader(void *arg);
void* writer(void *arg);
void read_data_base();
void write_data_base();

volatile int running = 1;

void s_handler(int sig) {
   running=0;
}

int main() {
    signal(SIGINT, s_handler);
    sem_init(&mutex, 0, 1);
    sem_init(&db, 0, 1);

    int writersCount;
    int readersCount;

    treatInput("Expected number of readers ", "Please type only numbers, idiot!", &readersCount);
    treatInput("Expected number of writers ", "Please type only numbers, idiot!", &writersCount);

    pthread_t readers[readersCount], writers[writersCount];
    int i;
    
    for (i = 0; i < readersCount; i++) {
        pthread_create(&readers[i], NULL, reader, (void*) (intptr_t)i);
    }

    for (i = 0; i < writersCount; i++) {
        pthread_create(&writers[i], NULL, writer, (void*) (intptr_t)i);
    }

    int rc;
    for (i = 0; i < readersCount; i++) {
        rc = pthread_join(readers[i], NULL);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for (i = 0; i < writersCount; i++) {
        rc = pthread_join(writers[i], NULL);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    sem_destroy(&mutex);
    sem_destroy(&db);
    
    printf("Goodbye!\n");
    return 0;
}

void* reader(void *arg) {
    int i = (intptr_t) arg;

    while(running) {               
        sem_wait(&mutex);       /* obtém acesso exclusivo à seção crítica */
        rc = rc + 1;            /* incrementa o número atual de leitores */

        if (rc == 1) {          /* se este for o primeiro leitor... */
            sem_wait(&db);      /* adquire acesso aos dados */
        }

        sem_post(&mutex);       /* libera o acesso exclusivo a seção crítica */
        read_data_base(i);      /* acesso aos dados */
        sem_wait(&mutex);       /* obtém acesso exclusivo a seção crítica */
        rc = rc - 1;            /* decrementa o número atual de leitores */

        if (rc == 0) {          /* se este for o último leitor... */   
            sem_post(&db);      /* libera o acesso do escritor aos dados */
        }

        sem_post(&mutex);       /* libera o acesso exclusivo a seção crítica */
        sleep( rand() % 5);     /* caso descomentado e existam muitos leitores, o escritor morre de fome */
    }
    printf("Reader %d exiting!\n", i);
}

void* writer(void *arg) {
    int i = (intptr_t) arg;

    while(running) {               
        sem_wait(&db);          /* obtém acesso exclusivo aos dados */
        write_data_base(i);     /* atualiza os dados */
        sem_post(&db);          /* libera o acesso exclusivo aos dados */
        sleep( rand() % 5);
    }
    printf("Writer %d exiting!\n", i);
}

void read_data_base(int i) {
    printf("Reader %d read data %c!\n", i, data);
}

void write_data_base(int i) {
    if(i == 0) {
        data += 1;
    } else { 
        data += i;
    }
    printf("Writer %d writing data %c!\n", i, data);
}