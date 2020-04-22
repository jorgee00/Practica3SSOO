#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "queue.h"
#include <pthread.h>

#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
//To create a queue
queue* queue_init(int size){
    queue * q = (queue *) malloc(sizeof(queue));
    q->size = size;
    q->readIndex = 0;
    q->writeIndex = 0;
    q->length = 0;
    q->array =  malloc(sizeof(struct element)*size);
    for(int i = 0; i<size; i++){
        q->array[i] = *(struct element *)(malloc(sizeof(struct element)));
    }

    if(pthread_mutex_init(&q->esritura,NULL) !=0){
        ///TODO error
        write(0,"1\n", 2);
    }
    if(pthread_cond_init(&q->no_lleno, NULL) !=0){
        ///TODO error
        write(0,"1\n", 2);
    }
    if(pthread_cond_init(&q->no_vacio, NULL) !=0){
        ///TODO error
        write(0,"1\n", 2);
    }
    return q;
}


// To Enqueue an element
int queue_put(queue *q, struct element* x) {
    pthread_mutex_lock(&q->esritura);
    while(queue_full(q)){
        pthread_cond_wait(&q->no_lleno, &q->esritura);
    }
    write(1,"1\n", 2);
    q->array[q->writeIndex].time = x->time;
    q->array[q->writeIndex].type = x->type;
    q->writeIndex = (q->writeIndex+1)%q->size;
    q->length++;
    pthread_cond_signal(&q->no_vacio);
    pthread_mutex_unlock(&q->esritura);

    return 0;
}


// To Dequeue an element.
struct element* queue_get(queue *q) {
    pthread_mutex_lock(&q->esritura);
    while(queue_empty(q)){
        pthread_cond_wait(&q->no_vacio, &q->esritura);
    }
    struct element* element = malloc(sizeof(struct element));
    element->time = q->array[q->readIndex].time;
    element->type = q->array[q->readIndex].type;
    q->readIndex =(q->readIndex+1)%q->size;
    q->length--;
    write(1,"2\n", 2);
    pthread_cond_signal(&q->no_lleno);
    pthread_mutex_unlock(&q->esritura);
    return element;
}


//To check queue state
int queue_empty(queue *q){
    return q->length==0;
}

int queue_full(queue *q){
    return q->size==q->length;
}

//To destroy the queue and free the resources
int queue_destroy(queue *q){
    free(q);
    return 0;
}
