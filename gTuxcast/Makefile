CFLAGS=`xml2-config --cflags` `pkg-config --cflags gtk+-2.0`
LDFLAGS=`xml2-config --libs` `pkg-config --libs gtk+-2.0`

main: main.o support.o interface.o callbacks.o ../libraries/socket.o
	g++ -o gTuxcast $(LDFLAGS) main.o support.o interface.o callbacks.o

main.o: main.cpp
	g++ -c $(CFLAGS) main.cpp

support.o: support.cpp
	g++ -c $(CFLAGS) support.cpp

interface.o: interface.cpp
	g++ -c $(CFLAGS) interface.cpp

callbacks.o: callbacks.cpp
	g++ -c $(CFLAGS) callbacks.cpp

../libraries/socket.o: ../libraries/socket.cpp
	make -C ../libraries socket.o

clean:
	rm -f *.o gTuxcast
