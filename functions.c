#include "functions.h"

#include <stdio.h>
#include <malloc.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <time.h>

struct sigaction action;

int number_of_cores = 0;

stats * bufferRA[BUFFER_SIZE];
int headRA = 0;
int tailRA = 0;

percentage * bufferAP[BUFFER_SIZE];
int headAP = 0;
int tailAP = 0;

pthread_mutex_t mutexBufferRA;
sem_t semEmptyRA;
sem_t semFullRA;

pthread_mutex_t mutexBufferAP;
sem_t semEmptyAP;
sem_t semFullAP;

volatile sig_atomic_t done = 0;

clock_t timeR;
clock_t timeA;
clock_t timeP;

int workingR = 0;
int workingA = 0;
int workingP = 0;

void term(int signum) {
    done = 1;

    assert(headAP >= 0 && headAP < BUFFER_SIZE);
    assert(tailAP >= 0 && tailAP < BUFFER_SIZE);
    assert(headRA >= 0 && headRA < BUFFER_SIZE);
    assert(tailRA >= 0 && tailRA < BUFFER_SIZE);
    

    if (headRA <= tailRA) {
        for (int i = headRA + 1; i < tailRA; i++)
            free(bufferRA[i]);
    } else {
        for (int i = 0; i < tailRA; i++)
            free(bufferRA[i]);
        for (int i = headRA + 1; i < BUFFER_SIZE; i++)
            free(bufferRA[i]);
    }

    if (headAP <= tailAP) {
        for (int i = headAP + 1; i < tailAP; i++)
            free(bufferAP[i]);
    } else {
        for (int i = 0; i < tailAP; i++)
            free(bufferAP[i]);
        for (int i = headAP + 1; i < BUFFER_SIZE; i++)
            free(bufferAP[i]);
    }
}

int move_right(int x) {
    assert(x >= 0 && x < BUFFER_SIZE);

    x++;
    if (x == BUFFER_SIZE) x = 0;
    return x;
}

percentage * calculate(percentage * array) {
    assert(array != NULL);

    int neckRA = move_right(headRA);

    for (int i = 0; i < number_of_cores; i++) {
        int PrevIdle = bufferRA[headRA][i].idle + bufferRA[headRA][i].iowait;
        int Idle = bufferRA[neckRA][i].idle + bufferRA[neckRA][i].iowait;

        int PrevNonIdle = bufferRA[headRA][i].user + bufferRA[headRA][i].nice + bufferRA[headRA][i].system + bufferRA[headRA][i].irq
         + bufferRA[headRA][i].softirq + bufferRA[headRA][i].steal;
        int NonIdle = bufferRA[neckRA][i].user + bufferRA[neckRA][i].nice + bufferRA[neckRA][i].system + bufferRA[neckRA][i].irq
         + bufferRA[neckRA][i].softirq + bufferRA[neckRA][i].steal;

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
    return array;
}

void print() {
    assert(bufferAP != NULL);

    for (int i = 0; i < number_of_cores; i++) {
        printf("cpu%c: %d%%\n", bufferAP[headAP][i].core_number, bufferAP[headAP][i].p);
    }
    printf("-------------------------------------\n");
}

stats get_stats(int wantline) {
    FILE * file = fopen("/proc/stat", "r");
    assert(file != NULL);

    stats stats_temp = {'x', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    char cbuffer[1024];

    int line = 0;
    while(fgets(cbuffer, sizeof(cbuffer) - 1, file)) {
        if (cbuffer[0] != 'c') return stats_temp;
        if (line == wantline) {
            sscanf(cbuffer,
                "cpu%c %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu",
                &stats_temp.core_number, &stats_temp.user, &stats_temp.nice, &stats_temp.system, &stats_temp.idle, &stats_temp.iowait,
                &stats_temp.irq, &stats_temp.softirq, &stats_temp.steal, &stats_temp.guest, &stats_temp.guestnice);
        }                                    
        line++;
    }
    
    fclose(file);

    return stats_temp;
}

double check_time(clock_t t) {
    t = clock() - t;
    double time = t/CLOCKS_PER_SEC;
    return time;
}

void * readerFunc(void * args) {
    while (!done) {
        stats * array = malloc(sizeof(stats));
        assert(array != NULL);

        int array_size = 0;
        int i = 0;

        stats stats_temp = get_stats(i);

        while (stats_temp.core_number != 'x') {
            array_size++;
            stats * reallocated_array = realloc(array, array_size * sizeof(stats));
            assert(reallocated_array != NULL);         
            array = reallocated_array;
            array[i] = stats_temp;
            
            i++;
            stats_temp = get_stats(i);
        }
        
        sleep(1);

        number_of_cores = array_size;
        
        sem_wait(&semEmptyRA);
        pthread_mutex_lock(&mutexBufferRA);

        bufferRA[tailRA] = array;
        tailRA = move_right(tailRA);

        pthread_mutex_unlock(&mutexBufferRA);
        sem_post(&semFullRA);
        
        if (!workingR) {
            timeR = clock();
            workingR = 1;
        }
    }
}

void * analyzerFunc(void * args) {
    while (!done) {
        sem_wait(&semFullRA);
        pthread_mutex_lock(&mutexBufferRA);

        int v;
        sem_getvalue(&semEmptyRA, &v);
        if (v < BUFFER_SIZE - 1) {
            percentage * array = malloc(number_of_cores * sizeof(percentage));
            assert(array != NULL);
            array = calculate(array);

            sem_wait(&semEmptyAP);
            pthread_mutex_lock(&mutexBufferAP);

            bufferAP[tailAP] = array;
            tailAP = move_right(tailAP);

            free(bufferRA[headRA]);
            headRA = move_right(headRA);

            pthread_mutex_unlock(&mutexBufferAP);
            sem_post(&semFullAP);
            sem_post(&semEmptyRA);
        }

        pthread_mutex_unlock(&mutexBufferRA);

        if (!workingA) {
            timeA = clock();
            workingA = 1;
        }
    }
}

void * printerFunc(void * args) {
    while (!done) {
        sem_wait(&semFullAP);
        pthread_mutex_lock(&mutexBufferAP);

        if (!done) print();

        free(bufferAP[headAP]);
        headAP = move_right(headAP);

        sem_post(&semEmptyAP);
        pthread_mutex_unlock(&mutexBufferAP);

        if (!workingP) {
            timeP = clock();
            workingP = 1;
        }
    }
}

void * watchdogFunc(void * args) {
    action.sa_handler = term;
    while (!done) {
        if (workingR) {
            if (check_time(timeR) >= WAITING_SECONDS) {
                printf("Did not receive data from the Reader thread.\n");
                kill(getpid(), SIGTERM);
            }
            workingR = 0;
        }

        if (workingA) {
            if (check_time(timeA) >= WAITING_SECONDS) {
                printf("Did not receive data from the Analyzer thread.\n");
                kill(getpid(), SIGTERM);
            }
            workingA = 0;
        }

        if (workingP) {
            if (check_time(timeP) >= WAITING_SECONDS) {
                printf("Did not receive data from the Printer thread.\n");
                kill(getpid(), SIGTERM);
            }
            workingP = 0;
        }
    }
}