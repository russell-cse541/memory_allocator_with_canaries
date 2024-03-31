#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h> // Include string.h for strcpy
#include <sys/mman.h>

#define MEMORY_SIZE 1024

typedef struct Block {
    size_t size;
    char free;  // 0 for false (not free), 1 for true (free)
    struct Block* next;
    struct Block* prev; // Pointer to the previous block
} Block;

static Block* head = NULL;
static size_t available_memory = 0;

void initialize() {
    head = (Block*)mmap(NULL, MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (head == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    head->size = MEMORY_SIZE - sizeof(Block);
    head->free = 1; // Initially, all memory is free
    head->next = NULL;
    head->prev = NULL; // Previous pointer for the head is NULL
    available_memory = head->size;
}

static inline void attempt_merge_next_block(Block* current) {
    // Merge with the next block if it is free
    if (current->next && current->next->free) {
        current->size += sizeof(Block) + current->next->size;
        current->next = current->next->next;
        if (current->next) {
            current->next->prev = current; // Update previous pointer of the next block
        }
        available_memory += sizeof(Block); // Added by me
    }
}

static inline void attempt_merge_prev_block(Block* current) {
    // Merge with the previous block if it is free
    if (current->prev && current->prev->free) {
        current->prev->size += sizeof(Block) + current->size;
        current->prev->next = current->next;
        if (current->next) {
            current->next->prev = current->prev; // Update previous pointer of the next block
        }
        available_memory += sizeof(Block); // Added by me
    }
}

void* my_malloc(size_t size) {
    if (size == 0 || size > available_memory) {
        return NULL;
    }
    Block *current = head;
    char wasBlockSplit = 0;
    while (current) {
        if (current->free && current->size >= size) {
            if (current->size > size + sizeof(Block)) {
                wasBlockSplit = 1;
                Block *new_block = (Block*)((uint8_t*)current + sizeof(Block) + size);
                new_block->size = current->size - size - sizeof(Block);
                new_block->free = 1;
                new_block->next = current->next;
                new_block->prev = current; // Set previous pointer for the new block
                if (current->next) {
                    current->next->prev = new_block; // Update previous pointer of the next block
                }
                current->next = new_block;
                current->size = size; //added by me
            }
            current->free = 0; // Mark as not free
            available_memory -= wasBlockSplit ? size + sizeof(Block) : current->size; 
            return (void*)((uint8_t*)current + sizeof(Block));
        }
        current = current->next;
    }
    return NULL;
}

void my_free(void* ptr) {
    Block* current = (Block*)((uint8_t*)ptr - sizeof(Block));
    current->free = 1; // Mark as free
    available_memory += current->size;
    attempt_merge_next_block(current);
    attempt_merge_prev_block(current);
}

void display_blocks() {
    Block *current = head;
    int i = 0;
    while (current) {
        printf("Block %d: [Address: %p, Size: %lu, Free: %s]\n", ++i, (void*)current, current->size, current->free ? "true" : "false");
        current = current->next;
    }
}

int main() {
    initialize();

    // Allocate memory
    int *ptr1 = (int*)my_malloc(sizeof(int));
    *ptr1 = 10;

    // Allocate memory again
    char *ptr2 = (char*)my_malloc(50 * sizeof(char));
    strcpy(ptr2, "Hello, world!");

    // Free memory
    my_free(ptr1);

    // Allocate memory again
    double *ptr3 = (double*)my_malloc(sizeof(double));
    *ptr3 = 3.14;

    // Display allocated blocks
    display_blocks();

    return 0;
}
