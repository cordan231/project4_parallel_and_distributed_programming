CC=gcc
CFLAGS=-pthread -O3

all: pthreads_app

pthreads_app: main.c
	$(CC) $(CFLAGS) main.c -o pthreads_app

clean:
	rm -f pthreads_app