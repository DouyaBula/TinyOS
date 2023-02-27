all: ./code/fibo.c ./code/main.c
	gcc -c -I "./include" ./code/fibo.c -o ./code/fibo.o
	gcc -c -I "./include" ./code/main.c -o ./code/main.o
	gcc -I "./include" ./code/fibo.c ./code/main.c -o fibo

clean:
	rm ./code/fibo.o
	rm ./code/main.o
