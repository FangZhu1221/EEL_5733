mapper: test.o
	cc test.o -pthread -o test
mapper.o: test.c
	cc -c test.c -lpthread
clean:
	rm test.o
