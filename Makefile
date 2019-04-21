9cc: main.c 9cc.c
	gcc main.c 9cc.c -o main

test: main.c 9cc.c 
	gcc main.c 9cc.c -o main
	./main -test
	./test.sh

clean:
	rm -f 9cc main *.o *~ tmp*
