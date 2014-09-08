#malloc and free makefile by Joe Komosinski

all: mapred

mapred: main.c sorted-list.c mapredhead.h uthash.h
	gcc -g -o mapred main.c sorted-list.c mapredhead.h uthash.h -lpthread
	
clean:
	rm -rf *.o *.a mapred
