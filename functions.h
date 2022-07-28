#include <time.h>

#define BUFFER_SIZE 10
#define WAITING_SECONDS 2

typedef struct stats {
    char core_number;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guestnice;
} stats;

typedef struct percentage {
    char core_number;
    int p;
} percentage;

void term(int signum);

int move_right(int x);

percentage * calculate(percentage * array);

void print();

stats get_stats(int wantline);

double check_time(clock_t t);

void * readerFunc(void * args);

void * analyzerFunc(void * args);

void * printerFunc(void * args);

void * watchdogFunc(void * args);