#ifndef HEADER_FILE
#define HEADER_FILE


struct element {
  int type; //Tipo de maquina
  int time; //Tiempo de uso
};

typedef struct queue {
	// Define the struct yourself
    int size;//tamaño del buffer
	int length;//numeros de elemntos ocupados
	int readIndex;// Posicion de lectura
	int writeIndex;// Posicion de escritura
	struct element* array;//Array de elementos
    pthread_mutex_t write; //Mutex para manejar la concurrrencia a la hora de escribir
    pthread_cond_t no_full; //Variable condcion que indica que no esta lleno
    pthread_cond_t no_empty;  //Variable condcion que indica que no esta vacio

}queue;

queue* queue_init (int size);
int queue_destroy (queue *q);
int queue_put (queue *q, struct element* elem);
struct element * queue_get(queue *q);
int queue_empty (queue *q);
int queue_full(queue *q);


#endif
