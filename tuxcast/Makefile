main: tuxcast.o rss.o
	g++ -g -o tuxcast tuxcast.o rss.o ../config/config.o `xml2-config --libs` `curl-config --libs`

tuxcast.o: tuxcast.cpp
	g++ -c -g tuxcast.cpp `curl-config --cflags` `xml2-config --cflags`

rss.o: rss.cpp
	g++ -c -g rss.cpp `xml2-config --cflags`

clean:
	rm *.o tuxcast
