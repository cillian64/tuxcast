main: config.o tuxcast-config.o ../libraries/filestuff.o
	g++ -g -o tuxcast-config tuxcast-config.o config.o ../libraries/filestuff.o `xml2-config --libs`

clean:
	rm *.o
	rm tuxcast-config

config.o: config.cpp
	g++ -g -c config.cpp `xml2-config --cflags`

tuxcast-config.o: tuxcast-config.cpp
	g++ -g -c tuxcast-config.cpp

../libraries/filestuff.o: ../libraries/filestuff.cpp
	make filestuff.o -C ../libraries/
