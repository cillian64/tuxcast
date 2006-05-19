main:
	make  -C tuxcast

clean:
	make -C tuxcast clean

install:
	install -m 755 tuxcast/tuxcast /usr/bin
	install -m 755 config/tuxcast-config /usr/bin/tuxcast-config

uninstall:
	rm /usr/bin/tuxcast /usr/bin/tuxcast-config

