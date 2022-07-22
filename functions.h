typedef struct stats {
    char core_number;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guestnice;
} stats;

struct stats get_stats(int wantline);

void * readerFunc();