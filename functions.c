#include "functions.h"

#include <stdio.h>
#include <malloc.h>
#include <pthread.h>
#include <semaphore.h>

data buffer[10];
int count = 0;

pthread_mutex_t mutexBuffer;

sem_t semEmpty;
sem_t semFull;

struct stats get_stats(int wantline) {
    FILE* file = fopen("/proc/stat", "r");
    struct stats stats = {'x', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    if (file == NULL) {
        perror("Could not open stat file");
        return stats;
    }

    char cbuffer[1024];

    int line = 0;
    while(fgets(cbuffer, sizeof(cbuffer) - 1, file)) {
        if (cbuffer[0] != 'c') return stats;
        if (line == wantline) {
            sscanf(cbuffer,
                "cpu%c %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu",
                &stats.core_number, &stats.user, &stats.nice, &stats.system, &stats.idle, &stats.iowait, &stats.irq, &stats.softirq, &stats.steal, &stats.guest, &stats.guestnice);
        }                                    
        line++;
    }
    
    fclose(file);

    return stats;
}

void * readerFunc(void *args) {
    while (1) {
        stats *array = malloc(sizeof *array);
        int arr_size = 1;
        array[0] = get_stats(0);
        int i = 1;
        stats stats1;
        do {
            stats1 = get_stats(i);
            if (stats1.core_number != 'x')
            {
                arr_size++;
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
        
        sleep(1);

        data *data = malloc(sizeof *data);
        data->stats = array;
        data->number_of_cores = arr_size;
        
        sem_wait(&semEmpty);

        pthread_mutex_lock(&mutexBuffer);

        buffer[count] = *data;
        count++;

        pthread_mutex_unlock(&mutexBuffer);

        sem_post(&semFull);

        free(data);
        free(array);
    }
}

void * analyzerFunc(void *args) {
    while (1) {
        data y;
        sem_wait(&semFull);
        pthread_mutex_lock(&mutexBuffer);
        y = buffer[0];
        count--;
        int k = 0;
        while (k < count) {        
            buffer[k]=buffer[k+1];
            k++;
            printf("%d\n", k);
        }
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semEmpty);
        printf("test: %lld\n", y.stats[2].user);
    }
}