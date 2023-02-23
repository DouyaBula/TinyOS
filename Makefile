.PHONY: clean

out: calc case_all
	./calc < case_all > out

# Your code here.
case_add: casegen.c
	gcc casegen.c -o casegen
	./casegen add 100 > case_add
case_sub: casegen.c
	gcc casegen.c -o casegen
	./casegen sub 100 > case_sub
case_mul: casegen.c
	gcc casegen.c -o casegen
	./casegen mul 100 > case_mul
case_div: casegen.c
	gcc casegen.c -o casegen
	./casegen div 100 > case_div
case_all: case_add case_sub case_mul case_div
	touch case_all
	cat case_add >> case_all
	cat case_sub >> case_all
	cat case_mul >> case_all
	cat case_div >> case_all
clean:
	rm -f out calc casegen case_*
