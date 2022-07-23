#include "functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

extern pthread_mutex_t mutexBuffer;

extern sem_t semEmpty;
extern sem_t semFull;

int main() {
    pthread_t Reader;
    pthread_t Analyzer;

    pthread_mutex_init(&mutexBuffer, NULL);

    sem_init(&semEmpty, 0, 10);
    sem_init(&semFull, 0, 0);

    pthread_create(&Reader, NULL, readerFunc, NULL);
    pthread_create(&Analyzer, NULL, analyzerFunc, NULL);
    pthread_join(Reader, NULL);
    pthread_join(Analyzer, NULL);

    sem_destroy(&semEmpty);
    sem_destroy(&semFull);

    pthread_mutex_destroy(&mutexBuffer);
    
    return 0;
}