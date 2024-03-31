#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h> // Include string.h for strcpy
#include <sys/mman.h>
#include "memory_allocator/memory_allocator.h"


int main() {
    initialize_memory_pool(1024);

    // Allocate memory
    int *ptr1 = (int*)mymalloc(sizeof(int));
    *ptr1 = 10;

    // Allocate memory again
    char *ptr2 = (char*)mymalloc(50 * sizeof(char));
    strcpy(ptr2, "Hello, world!");

    // Free memory
    myfree(ptr1);

    // Allocate memory again
    double *ptr3 = (double*)mymalloc(sizeof(double));
    *ptr3 = 3.14;

    // Display allocated blocks
    display_blocks();

    destroy_memory_pool();

    return 0;
}
