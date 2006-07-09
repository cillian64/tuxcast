main:
	make -C tuxcast

gTuxcast: gTuxcast/gTuxcast
	echo ""


gTuxcast/gTuxcast:
	# ** WARNING, GTUXCAST IS NOT READY FOR USE YET!  IT DOES *NOT* WORK.  USE IT AT YOUR OWN RISK**"
	# ** In 10 seconds, gTuxcast will compile, press Ctrl+C to cancel **"
	sleep 10
	make -C gTuxcast

clean:
	make -C tuxcast clean
	make -C libraries clean
	make -C gTuxcast clean

install: main
	install -m 755 tuxcast/tuxcast /usr/bin/tuxcast
	install -m 755 config/tuxcast-config /usr/bin/tuxcast-config
	
gTuxcast-install: gTuxcast
	install -m 755 gTuxcast/gTuxcast /usr/bin/gTuxcast

uninstall:
	rm -f /usr/bin/tuxcast /usr/bin/tuxcast-config 

gTuxcast-uninstall:
	rm -f /usr/bin/gTuxcast

