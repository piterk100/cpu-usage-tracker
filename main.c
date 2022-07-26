#include "functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

extern pthread_mutex_t mutexBufferRA;
extern sem_t semEmptyRA;
extern sem_t semFullRA;

extern pthread_mutex_t mutexBufferAP;
extern sem_t semEmptyAP;
extern sem_t semFullAP;

int main() {
    pthread_t Reader;
    pthread_t Analyzer;
    pthread_t Printer;

    pthread_mutex_init(&mutexBufferRA, NULL);
    sem_init(&semEmptyRA, 0, 10);
    sem_init(&semFullRA, 0, 0);

    pthread_mutex_init(&mutexBufferAP, NULL);
    sem_init(&semEmptyAP, 0, 10);
    sem_init(&semFullAP, 0, 0);

    pthread_create(&Reader, NULL, readerFunc, NULL);
    pthread_create(&Analyzer, NULL, analyzerFunc, NULL);
    pthread_create(&Printer, NULL, printerFunc, NULL);

    pthread_join(Reader, NULL);
    pthread_join(Analyzer, NULL);
    pthread_join(Printer, NULL);

    sem_destroy(&semEmptyRA);
    sem_destroy(&semFullRA);
    pthread_mutex_destroy(&mutexBufferRA);

    sem_destroy(&semEmptyAP);
    sem_destroy(&semFullAP);
    pthread_mutex_destroy(&mutexBufferAP);
    
    return 0;
}