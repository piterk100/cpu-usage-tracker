#include "functions.h"

#include <stdio.h>
#include <malloc.h>

struct stats get_stats(int wantline) {
    FILE* file = fopen("/proc/stat", "r");
    struct stats stats = {'x', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    if (file == NULL) {
        perror("Could not open stat file");
        return stats;
    }

    char buffer[1024];

    int line = 0;
    while(fgets(buffer, sizeof(buffer) - 1, file)) {
        if (buffer[0] != 'c') return stats;
        if (line == wantline) {
            sscanf(buffer,
                "cpu%c %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu",
                &stats.core_number, &stats.user, &stats.nice, &stats.system, &stats.idle, &stats.iowait, &stats.irq, &stats.softirq, &stats.steal, &stats.guest, &stats.guestnice);
        }                                    
        line++;
    }
    
    fclose(file);

    return stats;
}

void * readerFunc() {
    stats *array = malloc(sizeof *array);
    int arr_size = 1;
    array[0] = get_stats(0);
    int i = 1;
    stats stats1;
    do {
        stats1 = get_stats(i);
        if (stats1.core_number != 'x')
        {
            arr_size += 1;
            stats* reallocated_array = realloc(array, arr_size * sizeof(stats));
            if (reallocated_array) {
                array = reallocated_array;
            } else {
                printf("Error");
            }
            array[i] = stats1;
        }
        i++;
    } while (stats1.core_number != 'x');
    return array;
}