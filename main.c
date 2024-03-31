#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h> // Include string.h for strcpy
#include <sys/mman.h>


#ifdef USE_MY_MALLOC
    #define MALLOC mymalloc
    #define FREE myfree
    #include "memory_allocator/memory_allocator.h"
#else
    #define MALLOC malloc
    #define FREE free
#endif

int main() {

#ifdef USE_MY_MALLOC
    initialize_memory_pool(1024);
#endif


    int *ptr1 = (int*)MALLOC(sizeof(int));
    *ptr1 = 10;

    char *ptr2 = (char*)MALLOC(50 * sizeof(char));
    strcpy(ptr2, "Hello, world!");

    FREE(ptr2);


    double *ptr3 = (double*)MALLOC(sizeof(double));
    *ptr3 = 3.14;


    char *ptr4 = (char*)MALLOC(25 * sizeof(char));

    #ifdef USE_MY_MALLOC
    display_blocks();
    destroy_memory_pool();
    #endif

    return 0;
}
