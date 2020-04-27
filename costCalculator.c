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
    //Comprobamos el número de comentarios
    if(argc!=4){
        write(2,"El numero de argumentos no es valido\n",37);
        return -1;
    }
    
    //Pasamos fd entero los dos argumentos numéricos
    int numThreads = atoi(argv[2]);
    int bufferSize = atoi(argv[3]);
    
    //Comprobamos los parámetros
    if(numThreads<=0){
        write(2,"El numero de hilos debe de ser mayor que 0\n",43);
        return -1;
    }
    if(bufferSize<=0){
        write(2,"El tamaño del buffer ha de ser mayor que 0\n",43);
        return -1;
    }
    
    //Cerramos la entrada estándar y abrimos el fichero, de esta forma scanf leerá del fichero
    close(STDIN_FILENO);
    int fr = open(argv[1],O_RDONLY);
    
    //Comprobamos la apertura del fichero
    if(fr==-1){
        write(2,"No se ha podido abrir el fichero indicado\n",43);
        return -1;
    }


    //Leemos la primera línea del fichero y almacena el valor en operaciones, comprobamos la lectura
    int fd;
    fd = scanf("%d\n", &operaciones);
    if(fd == -1){
        write(STDERR_FILENO,"Error al leer del fichero indicado\n",35);
        return -1;
    }

    //Comprobamos el número de operaciones indicado
    if(operaciones<1){
        write(STDERR_FILENO,"Número de operaciones inválido\n",33);
        return -1;
    }

    //Creamos un array de elementos, y lo rellenamos con los datos del fichero
    int indices;
    buffer =  malloc(operaciones *  sizeof(struct element));
    for(int i = 0; i<operaciones; i++){
        buffer[i] = *(struct element*)malloc(sizeof(struct element));
        fd = scanf("%d %d %d\n", &indices, &buffer[i].type, &buffer[i].time);
        if(fd == -1) break;
        if(buffer[i].time <= 0 || buffer[i].type <= 0 ){
            fd = -2;
            break;
        }
    }
    //Comprobamos la lectura del fichero, de ser errónea se libera la memoria del buffer
    if(fd == -2){
        free(buffer);
        write(STDERR_FILENO,"El formato del fichero no es válido\n",37);
        return -1;
    }

    //Comprobamos la lectura del fichero, de ser errónea se libera la memoria del buffer
    if(fd == -1){
        free(buffer);
        write(STDERR_FILENO,"Error al leer del fichero indicado\n",35);
        return -1;
    }

    //Calculamos el número de hilos productores necesarios para la ejecución y creamos un array de hilos
    int numHilos = (operaciones>numThreads)?numThreads:operaciones;
    pthread_t hilos [numHilos];

    //Estructura usada para pasar los índices de comienzo y fin de los elementos que tiene que leer del buffer de elementos a los productores
    struct args * Args;
    int rodaja = 0;
    int restante = operaciones;

    //Inicializamos el buffer circular
    q = queue_init(bufferSize);

    //Inicializamos los hilos productores, y les asignamos un índice de comienzo y de fin de lectura através de la estructura Args
    for(int i = 0; i< numHilos; i++){
        Args  = (struct args *) malloc(sizeof(struct args));
        restante = restante - rodaja;
        rodaja = (restante)/(numHilos-i);
        Args->start = operaciones -  restante;
        Args->end = operaciones -  restante + rodaja -1;
        pthread_create(&hilos[i], NULL, &productor, Args);
    }

    //Inicializamos el hilo productor
    pthread_t consumidor;
    pthread_create(&consumidor,NULL,&funcionConsumidor,NULL);

    //Recogemos todos los hilo productores
    for(int i = 0; i< numHilos; i++){
        pthread_join(hilos[i],NULL);
    }

    //Recogemos el resultado del hilo consumidor, lo imprimimos y liberamos la cola
    int *result;
    pthread_join(consumidor, (void **)&result);
    //Liberamos memoria
    free(buffer);
    queue_destroy(q);

    if(*result == -1)
        return -1;

    printf("Total: %i €.\n", *result);
    return 0;
}

//Hilo productor
void * productor(void *arg){
    struct args* args = arg;
    int start = args->start;
    int end = args->end;
    free(args);
    //Lee los elementos que le correspondan del buffer de elementos y los inserta en el buffer circular
    for(int i = start; i<=end; i++) {
        struct element* elem = &buffer[i];
        queue_put(q,elem);
    }
    pthread_exit(0);
}

//Hilo consumidor
void * funcionConsumidor(void * arg){
    int  *acumulador = (int *) malloc(sizeof(int));
    *acumulador = 0;
    //Itera tantas veces como operaciones se hayan leido del fichero
    //Extrae los elementos del buffer circular, los procesa y los suma
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
                write(STDOUT_FILENO,"Tipo de máquina inválido\n",27);
                pthread_exit((void *) -1);
        }
        //Liberamos el elemento extraido de la cola
        free(elem);
    }
    pthread_exit(acumulador);
}