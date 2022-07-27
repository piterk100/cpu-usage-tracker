typedef struct stats {
    char core_number;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guestnice;
} stats;

typedef struct percentage {
    char core_number;
    int p;
} percentage;

void term(int signum);

stats get_stats(int wantline);

void * readerFunc(void *args);

void * analyzerFunc(void *args);

void * printerFunc(void *args);