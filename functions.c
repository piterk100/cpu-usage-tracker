#include "functions.h"

#include <stdio.h>
#include <malloc.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

dataRA bufferRA[10];
int countRA = 0;

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

        dataRA *dataRA = malloc(sizeof *dataRA);
        dataRA->stats = array;
        dataRA->number_of_cores = arr_size;
        
        sem_wait(&semEmpty);
        pthread_mutex_lock(&mutexBuffer);

        bufferRA[countRA].stats = dataRA->stats;
        bufferRA[countRA].number_of_cores = dataRA->number_of_cores;
        countRA++;

        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull);

        free(dataRA);
    }
}

void * analyzerFunc(void *args) {
    while (1) {
        dataRA *y1;
        dataRA *y2;

        sem_wait(&semFull);
        pthread_mutex_lock(&mutexBuffer);

        int v;
        sem_getvalue(&semEmpty, &v);
        if (v < 9) {
            y1 = &bufferRA[0];
            y2 = &bufferRA[1];
            countRA--;
            
            /*printf("test: %lld\n", y2->stats[0].user);
            printf("cpu%c %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld\n",
                y1->stats[0].core_number, y1->stats[0].user, y1->stats[0].nice, y1->stats[0].system, y1->stats[0].idle, y1->stats[0].iowait, y1->stats[0].irq, y1->stats[0].softirq, y1->stats[0].steal, y1->stats[0].guest, y1->stats[0].guestnice);
            
            */int i = 0;
            while (i < y1->number_of_cores) {
                int PrevIdle = y1->stats[i].idle + y1->stats[i].iowait;
                int Idle = y2->stats[i].idle + y2->stats[i].iowait;

                int PrevNonIdle = y1->stats[i].user + y1->stats[i].nice + y1->stats[i].system + y1->stats[i].irq + y1->stats[i].softirq + y1->stats[i].steal;
                int NonIdle = y2->stats[i].user + y2->stats[i].nice + y2->stats[i].system + y2->stats[i].irq + y2->stats[i].softirq + y2->stats[i].steal;

                int PrevTotal = PrevIdle + PrevNonIdle;
                int Total = Idle + NonIdle;

                int totald = Total - PrevTotal;
                int idled = Idle - PrevIdle;

                float a = (float)totald;
                float b = (float)idled;

                float CPU_Percentage = ((a - b)/a)*100;
                printf("cpu%c: %d\n", y2->stats[i].core_number, (int)CPU_Percentage);
                i++;
            }
            free(bufferRA[0].stats);
            int k = 0;
            while (k < countRA) {        
                bufferRA[k] = bufferRA[k+1];
                k++;
            }
            sem_post(&semEmpty);
        }

        pthread_mutex_unlock(&mutexBuffer);

        dataAP x;
    }
}

void * printerFunc(void *args) {
    while (1) {
        /* code */
    }   
}