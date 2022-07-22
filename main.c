#include "functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int main() {
    pthread_t Reader;
    void *out_void;
    stats * array;
    pthread_create(&Reader, NULL, readerFunc, NULL);
    pthread_join(Reader, &out_void);
    /*pthread_exit(NULL);*/

    array = out_void;
    printf("test: %c\n", array[2].core_number);
    free(array);

    return 0;
}