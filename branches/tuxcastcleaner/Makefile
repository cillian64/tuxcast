main: tuxcastcleaner

GPP=g++
STRIP=\#strip
LDFLAGS=-g `xml2-config --libs` `curl-config --libs`
CFLAGS=-c -g `xml2-config --cflags` `curl-config --cflags`

tuxcastcleaner: tuxcastcleaner.o config.o rss.o libraries/filestuff.o
	$(GPP) $(LDFLAGS) -o tuxcastcleaner tuxcastcleaner.o config.o rss.o libraries/filestuff.o
	$(STRIP) -s tuxcastcleaner

clean: main
	rm -f tuxcastcleaner *.o
	make -C libraries clean

tuxcastcleaner.o: tuxcastcleaner.cpp
	g++ $(CFLAGS) tuxcastcleaner.cpp

config.o: config.cpp
	g++ $(CFLAGS) config.cpp

rss.o: rss.cpp
	g++ $(CFLAGS) rss.cpp

libraries/filestuff.o:
	make -C libraries filestuff.o
