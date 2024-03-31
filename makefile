CC = gcc
CFLAGS = -Wall -g -Imemory_allocator
LDFLAGS = -lm

OBJS = main.o memory_allocator/memory_allocator.o

main.o: main.c memory_allocator/memory_allocator.h
	$(CC) $(CFLAGS) -c main.c

memory_allocator/memory_allocator.o: memory_allocator/memory_allocator.c memory_allocator/memory_allocator.h
	$(CC) $(CFLAGS) -c memory_allocator/memory_allocator.c -o memory_allocator/memory_allocator.o

main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o main $(LDFLAGS)

clean:
	rm -f *.o memory_allocator/*.o main

test: clean main
	./main