main: config.o tuxcast-config.o
	g++ -o tuxcast-config tuxcast-config.o config.o `xml2-config --libs`

clean:
	rm *.o
	rm tuxcast-config

config.o: config.cpp
	g++ -c config.cpp `xml2-config --cflags`

tuxcast-config.o: tuxcast-config.cpp
	g++ -c tuxcast-config.cpp
