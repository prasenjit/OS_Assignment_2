#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

sem_t mutexGlobal, mutexWriter;
int data = 0, readerCount = 0;

void *reader_func(void *reader){
    sem_wait(&mutexGlobal);
    ++readerCount;
    if (readerCount == 1)
        sem_wait(&mutexWriter);
    sem_post(&mutexGlobal);
    printf("Reader: %d\t\tData Read: %d\n", ((int)reader), data);
    sleep(1);
    sem_wait(&mutexGlobal);
    --readerCount;
    if (readerCount == 0)
        sem_post(&mutexWriter);
    sem_post(&mutexGlobal);
}

void *writer_func(void *writer) {
    sem_wait(&mutexWriter);
    ++data;
    printf("Writer: %d\t\tData Written: %d\n", ((int)writer), data);
    sleep(1);
    sem_post(&mutexWriter);
}


int main(void) {
    pthread_t readerId[5], writerId[5];
    sem_init(&mutexGlobal, 0, 1);
    sem_init(&mutexWriter, 0, 1);

    int i;
    for (i = 0; i < 5; ++i) {
        pthread_create(&writerId[i], NULL, writer_func, (void *)i);
        pthread_create(&readerId[i], NULL, reader_func, (void *)i);
    }


    // pthread_create(&writerId[0], NULL, writer_func, (void *)0);
    // pthread_create(&readerId[0], NULL, reader_func, (void *)0);
    // pthread_create(&readerId[1], NULL, reader_func, (void *)1);
    // pthread_create(&readerId[2], NULL, reader_func, (void *)2);
    // pthread_create(&writerId[1], NULL, writer_func, (void *)1);
    // pthread_create(&writerId[2], NULL, writer_func, (void *)2);
    // pthread_create(&readerId[3], NULL, reader_func, (void *)3);
    // pthread_create(&readerId[4], NULL, reader_func, (void *)4);
    // pthread_create(&writerId[3], NULL, writer_func, (void *)3);
    // pthread_create(&writerId[4], NULL, writer_func, (void *)4);

    for (i = 0; i < 3; ++i) {
        pthread_join(writerId[i], NULL);
        pthread_join(readerId[i], NULL);
    }


    return 0;
}