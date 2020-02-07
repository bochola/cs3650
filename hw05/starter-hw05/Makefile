
tokens: tokens.c svec.o
	gcc -g -Wall -o tokens tokens.c svec.o

svec.o: svec.c svec.h
	gcc -g -Wall -c -o svec.o svec.c

clean:
	rm -rf tmp tokens *.o

.PHONY: all clean

