#include "functions.h"

#include <stdio.h>
#include <malloc.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

int number_of_cores = 0;

stats * bufferRA[10];
int countRA = 0;

percentage * bufferAP[10];
int countAP = 0;

pthread_mutex_t mutexBufferRA;
pthread_mutex_t mutexBufferAP;

sem_t semEmptyRA;
sem_t semFullRA;

sem_t semEmptyAP;
sem_t semFullAP;

struct stats get_stats(int wantline) {
    FILE* file = fopen("/proc/stat", "r");
    stats statsx = {'x', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    if (file == NULL) {
        perror("Could not open stat file");
        return statsx;
    }

    char cbuffer[1024];

    int line = 0;
    while(fgets(cbuffer, sizeof(cbuffer) - 1, file)) {
        if (cbuffer[0] != 'c') return statsx;
        if (line == wantline) {
            sscanf(cbuffer,
                "cpu%c %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu",
                &statsx.core_number, &statsx.user, &statsx.nice, &statsx.system, &statsx.idle, &statsx.iowait, &statsx.irq, &statsx.softirq, &statsx.steal, &statsx.guest, &statsx.guestnice);
        }                                    
        line++;
    }
    
    fclose(file);

    return statsx;
}

void * readerFunc(void *args) {
    while (1) {
        stats * array = malloc(sizeof * array);
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

        number_of_cores = arr_size;
        
        sem_wait(&semEmptyRA);
        pthread_mutex_lock(&mutexBufferRA);

        bufferRA[countRA] = array;
        countRA++;

        pthread_mutex_unlock(&mutexBufferRA);
        sem_post(&semFullRA);
    }
}

void * analyzerFunc(void *args) {
    while (1) {
        percentage * array = malloc(number_of_cores * (sizeof * array));

        sem_wait(&semFullRA);
        pthread_mutex_lock(&mutexBufferRA);

        int v;
        sem_getvalue(&semEmptyRA, &v);
        if (v < 9) {
            countRA--;      
            int i = 0;
            while (i < number_of_cores) {
                int PrevIdle = bufferRA[0][i].idle + bufferRA[0][i].iowait;
                int Idle = bufferRA[1][i].idle + bufferRA[1][i].iowait;

                int PrevNonIdle = bufferRA[0][i].user + bufferRA[0][i].nice + bufferRA[0][i].system + bufferRA[0][i].irq + bufferRA[0][i].softirq + bufferRA[0][i].steal;
                int NonIdle = bufferRA[1][i].user + bufferRA[1][i].nice + bufferRA[1][i].system + bufferRA[1][i].irq + bufferRA[1][i].softirq + bufferRA[1][i].steal;

                int PrevTotal = PrevIdle + PrevNonIdle;
                int Total = Idle + NonIdle;

                int totald = Total - PrevTotal;
                int idled = Idle - PrevIdle;

                float a = (float)totald;
                float b = (float)idled;

                float CPU_Percentage = ((a - b)/a)*100;
                array[i].p = (int)CPU_Percentage;
                array[i].core_number = bufferRA[1][i].core_number;
                i++;
            }
            sem_wait(&semEmptyAP);
            pthread_mutex_lock(&mutexBufferAP);

            bufferAP[countAP] = array;
            countAP++;

            pthread_mutex_unlock(&mutexBufferAP);
            sem_post(&semFullAP);

            free(bufferRA[0]);
            int k = 0;
            while (k < countRA) {        
                bufferRA[k] = bufferRA[k+1];
                k++;
            }
            sem_post(&semEmptyRA);
        }
        pthread_mutex_unlock(&mutexBufferRA);
    }
}

void * printerFunc(void *args) {
    while (1) {
        sem_wait(&semFullAP);
        pthread_mutex_lock(&mutexBufferAP);

        int i = 0;
        while (i < number_of_cores) {
            printf("cpu%c: %d%%\n", bufferAP[0][i].core_number, bufferAP[0][i].p);
            i++;
        }
        printf("-------------------------------------\n");

        countAP--;
        free(bufferAP[0]);

        int k = 0;
        while (k < countAP) {        
            bufferAP[k] = bufferAP[k+1];
            k++;
        }

        sem_post(&semEmptyAP);
        pthread_mutex_unlock(&mutexBufferAP);
    }   
}