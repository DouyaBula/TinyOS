all: fibo.c main.c
	gcc -c -I "../include" fibo.c
	gcc -c -I "../include" main.c
	gcc -I "../include" fibo.c main.c -o ../fibo
clean: fibo.o main.o
	rm fibo.o
	rm main.o
