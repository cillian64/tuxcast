main: config.o tuxcast-config.o
	g++ -g -o tuxcast-config tuxcast-config.o config.o `xml2-config --libs` -lboost_filesystem

clean:
	rm *.o
	rm tuxcast-config

config.o: config.cpp
	g++ -g -c config.cpp `xml2-config --cflags`

tuxcast-config.o: tuxcast-config.cpp
	g++ -g -c tuxcast-config.cpp
