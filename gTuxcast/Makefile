CFLAGS=`xml2-config --cflags` `pkg-config --cflags gtk+-2.0`
LDFLAGS=`xml2-config --libs` `pkg-config --libs gtk+-2.0`

main: main.o support.o interface.o callbacks.o
	gcc -o gTuxcast $(LDFLAGS) main.o support.o interface.o callbacks.o

main.o: main.c
	gcc -c $(CFLAGS) main.c

support.o: support.c
	gcc -c $(CFLAGS) support.c

interface.o: interface.c
	gcc -c $(CFLAGS) interface.c

callbacks.o: callbacks.c
	gcc -c $(CFLAGS) callbacks.c

clean:
	rm -f *.o gTuxcast
