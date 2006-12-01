main:
	make -C tuxcast
	#make -C plugins

clean:
	make -C tuxcast clean
	make -C libraries clean
	#make -C plugins clean

install: main
	install -m 755 tuxcast/tuxcast /usr/bin/tuxcast
	install -m 755 config/tuxcast-config /usr/bin/tuxcast-config
	#install -m 755 plugins/tuxcast-amarok /usr/bin/tuxcast-amarok
	

uninstall:
	rm -f /usr/bin/tuxcast /usr/bin/tuxcast-config #/usr/bin/tuxcast-amarok


