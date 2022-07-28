#include "functions.h"

#include <stdio.h>
#include <malloc.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int number_of_cores = 0;

stats * bufferRA[BUFFER_RA_SIZE];
int headRA = 0;
int tailRA = 0;

percentage * bufferAP[BUFFER_AP_SIZE];
int headAP = 0;
int tailAP = 0;

pthread_mutex_t mutexBufferRA;
sem_t semEmptyRA;
sem_t semFullRA;

pthread_mutex_t mutexBufferAP;
sem_t semEmptyAP;
sem_t semFullAP;

volatile sig_atomic_t done = 0;

void term(int signum) {
    done = 1;

    if (headRA <= tailRA) {
        for (int i = headRA + 1; i < tailRA; i++)
            free(bufferRA[i]);
    } else {
        for (int i = 0; i < tailRA; i++)
            free(bufferRA[i]);
        for (int i = headRA + 1; i < BUFFER_RA_SIZE; i++)
            free(bufferRA[i]);
    }

    if (headAP <= tailAP) {
        for (int i = headAP + 1; i < tailAP; i++)
            free(bufferAP[i]);
    } else {
        for (int i = 0; i < tailAP; i++)
            free(bufferAP[i]);
        for (int i = headAP + 1; i < BUFFER_AP_SIZE; i++)
            free(bufferAP[i]);
    }
}

struct stats get_stats(int wantline) {
    FILE* file = fopen("/proc/stat", "r");
    stats stats_temp = {'x', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    if (file == NULL) {
        perror("Could not open stat file");
        return stats_temp;
    }

    char cbuffer[1024];

    int line = 0;
    while(fgets(cbuffer, sizeof(cbuffer) - 1, file)) {
        if (cbuffer[0] != 'c') return stats_temp;
        if (line == wantline) {
            sscanf(cbuffer,
                "cpu%c %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu",
                &stats_temp.core_number, &stats_temp.user, &stats_temp.nice, &stats_temp.system, &stats_temp.idle, &stats_temp.iowait, &stats_temp.irq, &stats_temp.softirq, &stats_temp.steal, &stats_temp.guest, &stats_temp.guestnice);
        }                                    
        line++;
    }
    
    fclose(file);

    return stats_temp;
}

void * readerFunc(void *args) {
    while (!done) {
        stats * array = malloc(sizeof(stats));
        int array_size = 1;
        array[0] = get_stats(0);
        int i = 1;
        stats stats_temp;
        do {
            stats_temp = get_stats(i);
            if (stats_temp.core_number != 'x')
            {
                array_size++;
                stats * reallocated_array = realloc(array, array_size * sizeof(stats));
                if (reallocated_array) {
                    array = reallocated_array;
                } else {
                    printf("Error");
                }
                array[i] = stats_temp;
            }
            i++;
        } while (stats_temp.core_number != 'x');
        
        sleep(1);

        number_of_cores = array_size;
        
        sem_wait(&semEmptyRA);
        pthread_mutex_lock(&mutexBufferRA);

        bufferRA[tailRA] = array;
        tailRA++;
        if (tailRA == BUFFER_RA_SIZE) tailRA = 0;

        pthread_mutex_unlock(&mutexBufferRA);
        sem_post(&semFullRA);
    }
}

void * analyzerFunc(void *args) {
    while (!done) {
        sem_wait(&semFullRA);
        pthread_mutex_lock(&mutexBufferRA);

        int v;
        sem_getvalue(&semEmptyRA, &v);
        if (v < BUFFER_RA_SIZE - 1) {
            percentage * array = malloc(number_of_cores * sizeof(percentage));
            
            int neckRA = headRA + 1;
            if (neckRA == BUFFER_RA_SIZE) neckRA = 0;

            for (int i = 0; i < number_of_cores; i++) {
                int PrevIdle = bufferRA[headRA][i].idle + bufferRA[headRA][i].iowait;
                int Idle = bufferRA[neckRA][i].idle + bufferRA[neckRA][i].iowait;

                int PrevNonIdle = bufferRA[headRA][i].user + bufferRA[headRA][i].nice + bufferRA[headRA][i].system + bufferRA[headRA][i].irq + bufferRA[headRA][i].softirq + bufferRA[headRA][i].steal;
                int NonIdle = bufferRA[neckRA][i].user + bufferRA[neckRA][i].nice + bufferRA[neckRA][i].system + bufferRA[neckRA][i].irq + bufferRA[neckRA][i].softirq + bufferRA[neckRA][i].steal;

                int PrevTotal = PrevIdle + PrevNonIdle;
                int Total = Idle + NonIdle;

                int totald = Total - PrevTotal;
                int idled = Idle - PrevIdle;

                float a = (float)totald;
                float b = (float)idled;

                float CPU_Percentage = ((a - b)/a)*100;
                array[i].p = (int)CPU_Percentage;
                array[i].core_number = bufferRA[neckRA][i].core_number;
            }
            sem_wait(&semEmptyAP);
            pthread_mutex_lock(&mutexBufferAP);

            bufferAP[tailAP] = array;
            tailAP++;
            if (tailAP == BUFFER_AP_SIZE) tailAP = 0;

            pthread_mutex_unlock(&mutexBufferAP);
            sem_post(&semFullAP);

            free(bufferRA[headRA]);
            headRA++;
            if (headRA == BUFFER_RA_SIZE) headRA = 0;
            
            sem_post(&semEmptyRA);
        }
        pthread_mutex_unlock(&mutexBufferRA);
    }
}

void * printerFunc(void *args) {
    while (!done) {
        sem_wait(&semFullAP);
        pthread_mutex_lock(&mutexBufferAP);

        for (int i = 0; i < number_of_cores; i++) {
            printf("cpu%c: %d%%\n", bufferAP[headAP][i].core_number, bufferAP[headAP][i].p);
        }
        printf("-------------------------------------\n");

        free(bufferAP[headAP]);
        headAP++;
        if (headAP == BUFFER_AP_SIZE) headAP = 0;

        sem_post(&semEmptyAP);
        pthread_mutex_unlock(&mutexBufferAP);
    }
}