
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <pthread.h>
#include "queue.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


#define NUM_CONSUMERS 1

struct queue * q;


struct args {
    int start; //First index to read
    int end; //Last index to read
};
/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */
void * thread(void *arg);

int main (int argc, const char * argv[] ){
    if(argc!=4){
        ///TODO mensaje error
        return -1;
    }
    int numThreads = atoi(argv[2]);
    int bufferSize = atoi(argv[3]);
    if(numThreads<=0){
        ///TODO mensaje error
        return -1;
    }
    if(bufferSize<=0){
        ///TODO mensaje error
        return -1;
    }

    close(STDIN_FILENO);
    int fr = open(argv[1],O_RDONLY);
    if(fr==-1){
        ///TODO mensaje error
        return -1;
    }
    int operaciones = 0;
    int indices;
    int a;
    a = scanf("%d\n",&operaciones);
    if(a == -1){
        ///TODO mensaje error
        return -1;
    }

    if(operaciones<1){
        ///TODO
    }

    int *buffer =  malloc(operaciones *  sizeof(struct element));
    for(int i = 0; i<operaciones; i++){
        struct element elem;
        a = scanf("%d %d %d\n",&indices,&elem.type,&elem.time);
        if(a == -1) break;
    }

    if(a==-1){
        free(buffer);
        ///TODO
        return -1;
    }
    int numHilos = (operaciones>numThreads)?operaciones:numThreads;
    pthread_t hilos [numHilos];
    struct args * Args = (struct args *) malloc(sizeof(struct args));
    int rodaja = 0;
    int restante = operaciones;

    q = queue_init(bufferSize);

    for(int i = 0; i< numHilos; i++){
        restante = restante - rodaja;
        rodaja = (restante)/(numHilos-i);

        Args->start = operaciones -  restante;
        Args->end = operaciones -  restante + rodaja -1;
        pthread_create(&hilos[i],NULL,&thread,Args);
    }

    for(int i = 0; i< numHilos; i++){
        pthread_join(hilos[i],NULL);
    }

    int total = 0;
    printf("Total: %i €.\n", total);

    return 0;
}

void * thread(void *arg){
    for(int i = ((struct args*)arg)->start; i<((struct args*)arg)->end; i++){
        printf("%d",i);
    }
}


