mapper: pthread.o
	cc pthread.o -pthread -o pthread
mapper.o: pthread.c
	cc -c pthread.c -lpthread
clean:
	rm pthread.o
