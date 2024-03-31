#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "memory_allocator.h"

typedef struct Block { // Block structure to hold metadata
    size_t size; // Size of the block
    char free;  // 0=> Not free, 1=> Free
    struct Block* next; // Pointer to the next block
    struct Block* prev; // Pointer to the previous block
} Block;


static size_t MEMORY_SIZE = 0; // Total memory size
static Block* head = NULL; // Head of the memory pool
static size_t available_memory = 0;   // Quick reference to check if memory is available

// Function to initialize the memory pool
void initialize_memory_pool(size_t size) {
    MEMORY_SIZE = size; // Set the memory size
    head = (Block*)mmap(NULL, MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (head == MAP_FAILED) { // Check if mmap failed
        perror("mmap"); // Print error message
        exit(EXIT_FAILURE); // Exit the program
    }
    // Initialize the head block
    head->size = MEMORY_SIZE - sizeof(Block); // Set the size of the block
    head->free = 1; // Initially, all memory is free
    head->next = NULL; // Initial block as no next block
    head->prev = NULL; // Initial block as no previous block
    available_memory = head->size; // Set available memory
} // close initialize_memory_pool

// Function to destroy the memory pool
void destroy_memory_pool() {
    if (munmap(head, MEMORY_SIZE) == -1) { // Unmap the memory pool
        perror("munmap"); // Print error message
        exit(EXIT_FAILURE); // Exit the program
    }
} // close destroy_memory_pool

// Function to merge the next block if free
static inline void attempt_merge_next_block(Block* current) {
    // Merge with the next block if it is free
    if (current->next && current->next->free) { //chedk if next is defined and free
        current->size += sizeof(Block) + current->next->size; // combine two sizes
        current->next = current->next->next; // update next to remove middle block
        if (current->next) { // check if next is defined
            current->next->prev = current; // Update previous pointer of the next block
        } 
        available_memory += sizeof(Block); // reclaim space of removed metadata block
    }
} // close attempt_merge_next_block

// Function to merge the previous block if free
static inline void attempt_merge_prev_block(Block* current) {
    // Merge with the previous block if it is free
    if (current->prev && current->prev->free) { // check if previous is defined and free
        current->prev->size += sizeof(Block) + current->size; 
        current->prev->next = current->next; // Update next pointer of the previous block
        if (current->next) { // check if next is defined
            current->next->prev = current->prev; // Update previous pointer of the next block
        }
        available_memory += sizeof(Block); // reclaim space of removed metadata block
    }
} // close attempt_merge_prev_block

// Function to allocate memory from the pool
void* mymalloc(size_t size) {
    if (size == 0 || size > available_memory) { // quick check if size is feasible
        return NULL;
    }
    Block *current = head; // Start from the head
    char wasBlockSplit = 0; // Flag to check if a free block was split
    while (current) { // Iterate through the blocks
        if (current->free && current->size >= size) { //check if block is free and big enough
            if (current->size > size + sizeof(Block)) { // Check if block can be split
                wasBlockSplit = 1; // Set the split flag
                Block *new_block = (Block*)((uint8_t*)current + sizeof(Block) + size); // Create a new block and account for metadata block size
                new_block->size = current->size - size - sizeof(Block); // Set the size of the new block accounting for metadata block size
                new_block->free = 1; // Mark as free
                new_block->next = current->next; // set pointer for new middle block
                new_block->prev = current; // Set previous pointer for the new block
                if (current->next) { // check if next is defined
                    current->next->prev = new_block; // Update previous pointer of the next block
                }
                current->next = new_block; // next is new middle block
                current->size = size; // Set the size of the current block
            } // close if block can be split
            current->free = 0; // mark as not free
            //update available memory - if block was not split, size may be too large
            available_memory -= wasBlockSplit ? size + sizeof(Block) : current->size; 
            return (void*)((uint8_t*)current + sizeof(Block)); // Return the pointer to the user
        } // close if block is free and big enough
        current = current->next; // Move to the next block in while search
    } // close while loop
    return NULL; // Return NULL if no memory is available
} // close mymalloc

// Function to free memory
void myfree(void* ptr) {
    Block* current = (Block*)((uint8_t*)ptr - sizeof(Block)); // Get the metadata block from the pointer
    current->free = 1; // Mark as free
    available_memory += current->size; // Update available memory
    attempt_merge_next_block(current); // Attempt to merge with the next block
    attempt_merge_prev_block(current);  // Attempt to merge with the previous block
} // close myfree

// Function to display the blocks
// This function is only used for debugging purposes and should not be used in production
void display_blocks() {
    Block *current = head; // Start from the head
    int i = 0; // Counter for the block number
    while (current) { // Iterate through the blocks
        printf("Block %d: [Address: %p, Size: %lu, Free: %s]\n", ++i, (void*)current, current->size, current->free ? "true" : "false");
        current = current->next; // Move to the next block
    } // close while loop
} // close display_blocks

