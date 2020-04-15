
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
#include <semaphore.h>

#define NUM_CONSUMERS 1

struct queue * q;
sem_t sem1;

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
        write(2,"El numero de argumentos no es valido\n",37);
        return -1;
    }
    int numThreads = atoi(argv[2]);
    if(numThreads<=0){
        write(2,"El numero de hilos debe de ser mayor que 0\n",43);
        return -1;
    }
    int bufferSize = atoi(argv[3]);
    if(bufferSize<=0){
        write(2,"El tamaño del buffer ha de ser mayor que 0\n",43);
        return -1;
    }

    close(STDIN_FILENO);
    int fr = open(argv[1],O_RDONLY);
    if(fr==-1){
        write(2,"No se ha podido abrir el fichero indicado\n",43);
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

    struct element *buffer =  malloc(operaciones *  sizeof(struct element));
    for(int i = 0; i<operaciones; i++){
        struct element elem;
        a = scanf("%d %d %d\n",&indices,&elem.type,&elem.time);
        if(a == -1) break;
        buffer[i] = elem;
    }

    if(a==-1){
        free(buffer);
        ///TODO
        return -1;
    }
    int numHilos = (operaciones>numThreads)?numThreads:operaciones;
    pthread_t hilos [numHilos];
    struct args * Args = (struct args *) malloc(sizeof(struct args));
    int rodaja = 0;
    int restante = operaciones;

    q = queue_init(bufferSize);
    sem_init(&sem1,0,1);

    for(int i = 0; i< numHilos; i++){
        sem_wait(&sem1);
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
    struct args* args = arg;
    int start = args->start;
    int end = args->end;
    sem_post(&sem1);

    for(int i = start; i<end; i++){
        //printf("Hilo %d\n",i);
    }
    printf("Inicio: %i\n",start);
    printf("Fin: %i\n", end);
}


