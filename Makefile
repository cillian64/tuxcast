main:
	make -C tuxcast

clean:
	make -C tuxcast clean
	make -C libraries clean

install: main
	install -m 755 tuxcast/tuxcast /usr/bin/tuxcast
	install -m 755 config/tuxcast-config /usr/bin/tuxcast-config
	

uninstall:
	rm -f /usr/bin/tuxcast /usr/bin/tuxcast-config 


