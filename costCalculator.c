
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



/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */

int main (int argc, const char * argv[] ) {

    close(STDIN_FILENO);
    int fr = open(argv[1],O_RDONLY);
    int operaciones = 0;

    scanf("%d",&operaciones);

    printf("%d\n",operaciones);

    struct element elements[operaciones];


    int total = 0;
    printf("Total: %i €.\n", total);

    return 0;
}
