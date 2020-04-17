#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "queue.h"
#include <pthread.h>


//To create a queue
queue* queue_init(int size){
    queue * q = (queue *) malloc(sizeof(queue));
    q->size = size;
    q->readIndex = 0;
    q->writeIndex = 0;
    q->length = 0;
    q->array = malloc(sizeof(struct element)*size);
    if(pthread_mutex_init(&q->esritura,NULL) !=0){
        ///TODO error
    }
    if(pthread_cond_init(&q->empty,NULL)){
        ///TODO error
    }
    return q;
}


// To Enqueue an element
int queue_put(queue *q, struct element* x) {
    pthread_mutex_lock(&q->esritura);
    q->array[q->writeIndex].time = x->time;
    q->array[q->writeIndex].type = x->type;
    q->writeIndex = (q->writeIndex+1)%q->size;
    if(queue_empty(q))pthread_cond_signal(&q->empty);
    q->length++;
    if(q->length<q->size)pthread_mutex_unlock(&q->esritura);
    return 0;
}


// To Dequeue an element.
struct element* queue_get(queue *q) {
    while(queue_empty(q)){
        pthread_cond_wait(&q->empty, &q->esritura);
    }
    struct element* element;
    element->time = q->array[q->readIndex].time;
    element->type = q->array[q->readIndex].type;
    q->readIndex =(q->readIndex+1)%q->size;
    if(q->length==q->size)pthread_mutex_unlock(&q->esritura);
    q->length--;
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
