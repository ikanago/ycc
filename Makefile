CFLAGS=-Wall -std=c11
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

ycc: $(OBJS)
	$(CC) -o ycc $(OBJS) $(LDFLAGS)

$(OBJS): ycc.h

test: ycc
	./ycc --test
	./test.sh

debug: $(OBJS)
	$(CC) -g -o dycc $(SRCS) $(LDFLAGS)

debug_asm: $(OBJS)
	$(CC) -g -DDEBUG -o dycc $(SRCS) $(LDFLAGS)

clean:
	rm -f ycc dycc *.o *~ tmp*

format:
	clang-format -i *.c *.h