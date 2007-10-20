main:
	make -C tuxcast
	#make -C plugins

all: main

clean:
	make -C tuxcast clean
	make -C libraries clean
	rm -f version.o
	#make -C plugins clean

version.o: version.cpp
	g++ -c version.cpp

install: main
	strip -s tuxcast/tuxcast
	strip -s config/tuxcast-config
	install -m 755 tuxcast/tuxcast /usr/bin/tuxcast
	install -m 755 config/tuxcast-config /usr/bin/tuxcast-config
	#install -m 755 plugins/tuxcast-amarok /usr/bin/tuxcast-amarok
	

uninstall:
	rm -f /usr/bin/tuxcast /usr/bin/tuxcast-config #/usr/bin/tuxcast-amarok


