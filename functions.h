#include <pthread.h>

typedef struct stats {
    char core_number;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guestnice;
} stats;

typedef struct data {
    stats * stats;
    int number_of_cores;
} data;

struct stats get_stats(int wantline);

void * readerFunc(void *args);

void * analyzerFunc(void *args);