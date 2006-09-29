main: backend.o worker.o manager.o ../tuxcast/rss.o ../libraries/filestuff.o
	g++ -o tuxcast-backend backend.o worker.o manager.o ../tuxcast/rss.o ../libraries/filestuff.o -lpthread `curl-config --libs` `xml2-config --libs`

backend.o: backend.cpp
	g++ -g -c backend.cpp

worker.o: worker.cpp
	g++ -g -c worker.cpp

manager.o: manager.cpp
	g++ -g -c manager.cpp

../tuxcast/rss.o: ../tuxcast/rss.cpp
	make -C ../tuxcast rss.o

../libraries/filestuff.o: ../libraries/filestuff.cpp
	make -C ../libraries filestuff.o

clean:
	rm -f tuxcast-backend *.o
