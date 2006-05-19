main:
	make  -C tuxcast

clean:
	make -C tuxcast clean

install:
	cp tuxcast/tuxcast /usr/bin
	cp config/tuxcast-config /usr/bin/tuxcast-config /usr/bin

