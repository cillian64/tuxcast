main: tuxcast.o rss.o cleaner.o ../config.o ../libraries/filestuff.o tuxcast_functions.o ../version.o 
#../plugins/amarok-plugin.o
	g++ -g -o tuxcast tuxcast.o tuxcast_functions.o rss.o cleaner.o ../config/config.o ../libraries/filestuff.o ../version.o `xml2-config --libs` `curl-config --libs` # ../plugins/amarok-plugin.o -lsqlite3

tuxcast.o: tuxcast.cpp
	g++ -c -g tuxcast.cpp `xml2-config --cflags` `curl-config --cflags`

tuxcast_functions.o: tuxcast_functions.cpp
	g++ -c -g tuxcast_functions.cpp `curl-config --cflags` `xml2-config --cflags`

rss.o: rss.cpp
	g++ -c -g rss.cpp `xml2-config --cflags`

cleaner.o: cleaner.cpp
	g++ -c -g cleaner.cpp `xml2-config --cflags`

../config.o:	
	make -f ../config/Makefile -C ../config

../libraries/filestuff.o: ../libraries/filestuff.cpp
	make filestuff.o -C ../libraries/

../plugins/amarok-plugin.o: ../plugins/amarok-plugin.cpp
	make amarok-plugin.o -C ../plugins/

../version.o: ../version.cpp
	make version.o -C ..

clean:
	rm -f *.o tuxcast
	make -f ../config/Makefile -C ../config clean
