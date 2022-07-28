#include "functions.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>
#include <assert.h>

extern pthread_mutex_t mutexBufferRA;
extern sem_t semEmptyRA;
extern sem_t semFullRA;

extern pthread_mutex_t mutexBufferAP;
extern sem_t semEmptyAP;
extern sem_t semFullAP;

extern volatile sig_atomic_t done;

int main() {
    extern struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);

    pthread_t Reader;
    pthread_t Analyzer;
    pthread_t Printer;
    pthread_t Watchdog;

    pthread_mutex_init(&mutexBufferRA, NULL);
    sem_init(&semEmptyRA, 0, 10);
    sem_init(&semFullRA, 0, 0);

    pthread_mutex_init(&mutexBufferAP, NULL);
    sem_init(&semEmptyAP, 0, 10);
    sem_init(&semFullAP, 0, 0);

    assert(pthread_create(&Reader, NULL, readerFunc, NULL) == 0);
    assert(pthread_create(&Analyzer, NULL, analyzerFunc, NULL) == 0);
    assert(pthread_create(&Printer, NULL, printerFunc, NULL) == 0);
    assert(pthread_create(&Watchdog, NULL, watchdogFunc, NULL) == 0);

    pthread_join(Reader, NULL);
    pthread_join(Analyzer, NULL);
    pthread_join(Printer, NULL);
    pthread_join(Watchdog, NULL);

    sem_destroy(&semEmptyRA);
    sem_destroy(&semFullRA);
    pthread_mutex_destroy(&mutexBufferRA);

    sem_destroy(&semEmptyAP);
    sem_destroy(&semFullAP);
    pthread_mutex_destroy(&mutexBufferAP);
    
    return 0;
}