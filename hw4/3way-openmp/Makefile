CC = gcc
CFLAGS = -O2 -fopenmp

all: openmp_app

openmp_app: main.c
	$(CC) $(CFLAGS) -o openmp_app main.c

clean:
	rm -f openmp_app