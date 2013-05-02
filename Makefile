CC = g++
CFLAGS = -g

a.out: test.cpp UglyList.h
	$(CC) $(CFLAGS) test.cpp

clean:
	rm -f a.out

.PHONY: clean

test: a.out
	./a.out > test.log
	less test.log

.PHONY: test
