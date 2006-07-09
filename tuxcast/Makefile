main: tuxcast.o rss.o backend.o ../config.o ../libraries/filestuff.o ../libraries/socket.o
	g++ -g -o tuxcast tuxcast.o rss.o backend.o ../config/config.o ../libraries/filestuff.o ../libraries/socket.o `xml2-config --libs` `curl-config --libs`

tuxcast.o: tuxcast.cpp
	g++ -c -g tuxcast.cpp `curl-config --cflags` `xml2-config --cflags`

rss.o: rss.cpp
	g++ -c -g rss.cpp `xml2-config --cflags`

backend.o: backend.cpp
	g++ -c -g backend.cpp `xml2-config --cflags` `curl-config --cflags`

../config.o:	
	make -f ../config/Makefile -C ../config

../libraries/filestuff.o: ../libraries/filestuff.cpp
	make filestuff.o -C ../libraries/

../libraries/socket.o: ../libraries/socket.cpp
	make socket.o -C ../libraries/

clean:
	rm -f *.o tuxcast
	make -f ../config/Makefile -C ../config clean
