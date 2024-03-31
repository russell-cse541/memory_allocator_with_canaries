CC = gcc
CFLAGS = -Wall -g -Imemory_allocator
LDFLAGS = -lm

OBJS = main.o memory_allocator/memory_allocator.o
MYMALLOC_OBJS = main_mymalloc.o memory_allocator/memory_allocator.o

main.o: main.c memory_allocator/memory_allocator.h
	$(CC) $(CFLAGS) -c main.c

main_mymalloc.o: main.c memory_allocator/memory_allocator.h
	$(CC) $(CFLAGS) -DUSE_MY_MALLOC -c main.c -o main_mymalloc.o

memory_allocator/memory_allocator.o: memory_allocator/memory_allocator.c memory_allocator/memory_allocator.h
	$(CC) $(CFLAGS) -c memory_allocator/memory_allocator.c -o memory_allocator/memory_allocator.o

main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o main $(LDFLAGS)

main_mymalloc: $(MYMALLOC_OBJS)
	$(CC) $(CFLAGS) $(MYMALLOC_OBJS) -o main_mymalloc $(LDFLAGS)

clean:
	rm -f *.o memory_allocator/*.o main main_mymalloc

test: clean main
	./main

test_mymalloc: clean main_mymalloc
	./main_mymalloc