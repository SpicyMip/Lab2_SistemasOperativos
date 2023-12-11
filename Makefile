all: main

main:
	gcc -o main main.c
	./main
clean:
	rm -f main