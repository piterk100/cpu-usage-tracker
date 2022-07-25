#include <pthread.h>

typedef struct stats {
    char core_number;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guestnice;
} stats;

typedef struct dataRA {
    stats * stats;
    int number_of_cores;
} dataRA;

typedef struct dataAP {
    int * percentage;
    int number_of_cores;
} dataAP;

struct stats get_stats(int wantline);

void * readerFunc(void *args);

void * analyzerFunc(void *args);

void * printerFunc(void *args);