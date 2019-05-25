CFLAGS=-Wall -std=c11
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

test: 9cc
	./9cc -test
	./test.sh

debug: $(OBJS)
	$(CC) -g -o d9cc $(SRCS) $(LDFLAGS)

clean:
	rm -f 9cc  *.o *~ tmp*

format:
	clang-format -i *.c *.h