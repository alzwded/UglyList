CC = g++
CFLAGS = -g

a.out: test.cpp UglyList.h
	$(CC) $(CFLAGS) test.cpp

clean:
	rm -f a.out

.PHONY: clean

test.log: a.out
	./a.out > test.log

test: test.log a.out

vtest: test
	less test.log

.PHONY: test
.PHONY: vtest
