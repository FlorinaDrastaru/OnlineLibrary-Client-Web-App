CC=gcc
CFLAGS= -Wall -g 

client: client.o requests.o helpers.o buffer.o parson.o
	$(CC) -o client $^ $(CFLAGS) 

client.o: client.c 
	$(CC) client.c -c $(CFLAGS)

requests.o: requests.c
	$(CC) requests.c -c $(CFLAGS)

helpers.o: helpers.c
	$(CC) helpers.c -c $(CFLAGS)

buffer.o: buffer.c
	$(CC) buffer.c -c $(CFLAGS)

parson.o: parson.c
	$(CC) parson.c -c $(CFLAGS)
 
 

run: client
	./client

clean:
	rm -f *.o client
