#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "queue.h"
#include <pthread.h>
#include <unistd.h>


//Metodo para inicializar el buffer circular
queue* queue_init(int size){
    //Reservamos el tamaño de la estructura
    queue * q = (queue *) malloc(sizeof(queue));
    //Asignamos valores inciales
    q->size = size;
    q->readIndex = 0;
    q->writeIndex = 0;
    q->length = 0;
    //Declaramos el array y creamos todas las estructuras que contiene
    q->array = (struct element*) malloc(sizeof(struct element)*size);
    for(int i = 0; i<size; i++){
        q->array[i] = *(struct element *)(malloc(sizeof(struct element)));
    }
    //Declaramos el mutex que controla que no se pueda acceder simultaneamente albuffer circular y comprobamos su creacion
    if(pthread_mutex_init(&q->write, NULL) != 0)
        write(2,"Error inicializando el mutex\n", 29);
    //Declaramos las variables condicion que hacen que no se lea cuando esta vacio y no se escriba cuando esta lleno y comprobamos su creacion
    if(pthread_cond_init(&q->no_full, NULL) != 0)
        write(2,"Error inicializando la variable condicion no_full\n", 51);

    if(pthread_cond_init(&q->no_empty, NULL) != 0)
        write(2,"Error inicializando la variable condicion no_empty\n", 51);

    return q;
}


// Insertar un elemento en el buffer circular
int queue_put(queue *q, struct element* elem) {
    //Esperamos nuestro turno y bloqueamos para que otro no acceda
    pthread_mutex_lock(&q->write);
    //Esperamos a que el buffer no este lleno
    while(queue_full(q)){
        pthread_cond_wait(&q->no_full, &q->write);
    }
    //leemos el valor del elemento y lo insertamos en la estructura de la cola
    q->array[q->writeIndex].time = elem->time;
    q->array[q->writeIndex].type = elem->type;
    //Aumentamos el tamaño y la longitud actual del buffer
    q->writeIndex = (q->writeIndex+1)%q->size;
    q->length++;

    //Desbloqueamos el mutex y la variable condicion
    pthread_cond_signal(&q->no_empty);
    pthread_mutex_unlock(&q->write);

    return 0;
}


// Leemos un elemento del buffer circular
struct element* queue_get(queue *q) {
    //Esperamos nuestro turno y bloqueamos para que otro no acceda
    pthread_mutex_lock(&q->write);
    //Esperamos a que el buffer no este vacia
    while(queue_empty(q)){
        pthread_cond_wait(&q->no_empty, &q->write);
    }
    //Creamos el elemento a devolver y lo rellenamos
    struct element* element = malloc(sizeof(struct element));
    element->time = q->array[q->readIndex].time;
    element->type = q->array[q->readIndex].type;
    //Avanzamos uno en la posicion de lectura y restamos uno a la longitud actual del buffer
    q->readIndex =(q->readIndex+1)%q->size;
    q->length--;
    //Desbloqueamos el mutex y la variable condicion
    pthread_cond_signal(&q->no_full);
    pthread_mutex_unlock(&q->write);
    //DEvuelve el elemento
    return element;
}

//Te indica si la cola esta vacia
int queue_empty(queue *q){
    return q->length==0;
}

//Te indica si la cola esta llena
int queue_full(queue *q){
    return q->size==q->length;
}

//Destruye la cola y todos sus elementos
int queue_destroy(queue *q){
    free(q->array);
    pthread_cond_destroy(&q->no_empty);
    pthread_cond_destroy(&q->no_full);
    pthread_mutex_destroy(&q->write);
    free(q);
    return 0;
}
