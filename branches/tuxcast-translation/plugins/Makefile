main: tuxcast-amarok amarok-plugin.o
	echo Done

tuxcast-amarok: tuxcast-amarok.o amarok-plugin.o
	g++ -o tuxcast-amarok -g tuxcast-amarok.o amarok-plugin.o -lsqlite3
	# strip -s tuxcast-amarok

tuxcast-amarok.o: tuxcast-amarok.cpp amarok-plugin.cpp
	g++ -c -g tuxcast-amarok.cpp

amarok-plugin.o: amarok-plugin.cpp
	g++ -c -g amarok-plugin.cpp

clean:
	rm -f *.o tuxcast-amarok
