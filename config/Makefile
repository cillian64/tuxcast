main: config.o tuxcast-config.o ../libraries/filestuff.o ../version.o
	g++ -g -o tuxcast-config tuxcast-config.o config.o ../libraries/filestuff.o ../version.o `xml2-config --libs`

clean:
	rm -f *.o
	rm -f tuxcast-config

config.o: config.cpp
	g++ -g -c config.cpp `xml2-config --cflags`

tuxcast-config.o: tuxcast-config.cpp
	g++ -g -c tuxcast-config.cpp

../libraries/filestuff.o: ../libraries/filestuff.cpp
	make filestuff.o -C ../libraries/

../version.o: ../version.cpp
	make version.o -C ..
