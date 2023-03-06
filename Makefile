all: test.c
	gcc test.c -o test

run: test
	./test

clean: test
	rm -f test
