#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <pthread.h>
#include "queue.h"
#include <sys/types.h>

struct queue * q;
struct element *buffer;
int operaciones;

struct args {
    int start; //First index to read
    int end;   //Last index to read
};
/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */
void * productor(void *arg);
void * funcionConsumidor(void *arg);

int main (int argc, const char * argv[] ){
    if(argc!=4){
        write(2,"El numero de argumentos no es valido\n",37);
        return -1;
    }
    int numThreads = atoi(argv[2]);
    int bufferSize = atoi(argv[3]);
    if(numThreads<=0){
        write(2,"El numero de hilos debe de ser mayor que 0\n",43);
        return -1;
    }
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

    int indices;
    int a;
    a = scanf("%d\n",&operaciones);
    if(a == -1){
        write(STDERR_FILENO,"Error al leer del fichero indicado\n",35);
        return -1;
    }

    if(operaciones<1){
        write(STDERR_FILENO,"Número de operaciones inválido\n",32);
    }

    buffer =  malloc(operaciones *  sizeof(struct element));
    for(int i = 0; i<operaciones; i++){
        struct element elem;
        a = scanf("%d %d %d\n",&indices,&elem.type,&elem.time);
        if(a == -1) break;
        buffer[i] = elem;
    }

    if(a==-1){
        free(buffer);
        write(STDERR_FILENO,"Error al leer del fichero indicado\n",35);
        return -1;
    }

    int numHilos = (operaciones>numThreads)?numThreads:operaciones;
    pthread_t hilos [numHilos];
    struct args * Args;
    int rodaja = 0;
    int restante = operaciones;

    q = queue_init(bufferSize);

    for(int i = 0; i< numHilos; i++){
        Args  = (struct args *) malloc(sizeof(struct args));
        restante = restante - rodaja;
        rodaja = (restante)/(numHilos-i);
        Args->start = operaciones -  restante;
        Args->end = operaciones -  restante + rodaja -1;
        pthread_create(&hilos[i], NULL, &productor, Args);
    }
    pthread_t consumidor;
    pthread_create(&consumidor,NULL,&funcionConsumidor,NULL);
    for(int i = 0; i< numHilos; i++){
        pthread_join(hilos[i],NULL);
    }

    int *result;
    pthread_join(consumidor, (void **)&result);
    printf("Total: %i €.\n", *result);

    return 0;
}

void * productor(void *arg){
    struct args* args = arg;
    int start = args->start;
    int end = args->end;
    free(args);
    for(int i = start; i<=end; i++) {
        struct element* elem = &buffer[i];
        queue_put(q,elem);
    }
    pthread_exit(0);
}

void * funcionConsumidor(void * arg){
    int  *acumulador = (int *) malloc(sizeof(int));
    *acumulador = 0;
    for(int i =0; i<operaciones; i++){
        struct element * elem = queue_get(q);
        switch(elem->type){
            case 1:
                *acumulador = *acumulador + elem->time;
                break;
            case 2:
                *acumulador = *acumulador + 3*elem->time;
                break;
            case 3:
                *acumulador = *acumulador + 10*elem->time;
                break;
            default:
                write(STDOUT_FILENO,"Tipo de máquina inválido\n",25);
                pthread_exit((void *) -1);
        }
    }
    pthread_exit(acumulador);
}


