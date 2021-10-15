CC=gcc
CFLAGS=-std=c99 -Wall -Wextra
LOAD=load_balancer
SERVER=server
HASHTABLE=Hashtable
LINKEDLIST=LinkedList
LOAD_UTILS=load_balancer_utils

.PHONY: build clean

build: tema2

tema2: main.o $(LOAD).o $(SERVER).o $(HASHTABLE).o $(LINKEDLIST).o $(LOAD_UTILS).o
	$(CC) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $^ -c

$(SERVER).o: $(SERVER).c $(SERVER).h
	$(CC) $(CFLAGS) $^ -c

$(LOAD_UTILS).o: $(LOAD_UTILS).c $(LOAD_UTILS).h
	$(CC) $(CFLAGS) $^ -c

$(LOAD).o: $(LOAD).c $(LOAD).h load_balancer_utils.c load_balancer_utils.h
	$(CC) $(CFLAGS) $^ -c

$(LINKEDLIST).o: $(LINKEDLIST).c $(LINKEDLIST).h
	$(CC) $(CFLAGS) $^ -c

$(HASHTABLE).o: $(HASHTABLE).c $(HASHTABLE).h $(LINKEDLIST).c $(LINKEDLIST).h
	$(CC) $(CFLAGS) $^ -c

pack:
	zip -FSr 313CA_Raduta_Lavinia_Maria_Tema2.zip README Makefile *.c *.h

clean:
	rm -f *.o tema2 *.h.gch
