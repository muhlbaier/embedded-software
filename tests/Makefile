
INCLUDES = ../include
HAL_INCLUDES = ../hal/linux

BUFFER_C = ../src/buffer.c

CC = gcc

CFLAGS = -I $(INCLUDES) -I $(HAL_INCLUDES)

all: buffer

buffer.o: ${BUFFER_C}
	$(CC) $(CFLAGS) -c $(BUFFER_C) -o $@

buffer_tests.o: buffer_tests.c
	$(CC) $(CFLAGS) -c buffer_tests.c -o $@

buffer: buffer.o buffer_tests.o
	$(CC) $(CFLAGS) buffer.o buffer_tests.o -o buffer

clean:
	rm *.o buffer

