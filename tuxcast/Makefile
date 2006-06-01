main: tuxcast.o rss.o backend.o ../config.o
	g++ -g -o tuxcast tuxcast.o rss.o backend.o ../config/config.o `xml2-config --libs` `curl-config --libs` -lboost_filesystem

tuxcast.o: tuxcast.cpp
	g++ -c -g tuxcast.cpp `curl-config --cflags` `xml2-config --cflags`

rss.o: rss.cpp
	g++ -c -g rss.cpp `xml2-config --cflags`

backend.o: backend.cpp
	g++ -c -g backend.cpp `xml2-config --cflags`

../config.o:	
	make -f ../config/Makefile -C ../config

clean:
	rm *.o tuxcast
	make -f ../config/Makefile -C ../config clean
