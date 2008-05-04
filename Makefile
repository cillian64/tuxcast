include config.mk

.PHONY: main all clean updatetrans install uninstall

main:
	$(MAKE) -C tuxcast
	#$(MAKE) -C plugins
	$(MAKE) -C po

all: main

clean:
	$(MAKE) -C tuxcast clean
	$(MAKE) -C libraries clean
	#$(MAKE) -C plugins clean
	$(MAKE) -C po clean
	rm -f version.o

updatetrans:
	$(MAKE) -C po

version.o: version.cpp
	$(CXX) $(CXXFLAGS) -c version.cpp

install: main
	$(STRIP) -s tuxcast/tuxcast
	$(STRIP) -s config/tuxcast-config
	$(INSTALL) -D -m 755 tuxcast/tuxcast $(DESTDIR)$(PREFIX)/bin/tuxcast
	$(INSTALL) -D -m 755 config/tuxcast-config $(DESTDIR)$(PREFIX)/bin/tuxcast-config
	#$(INSTALL) -m 755 plugins/tuxcast-amarok $(DESTDIR)$(PREFIX)/bin/tuxcast-amarok
	$(INSTALL) -D man/tuxcast.1 $(MANDIR)
	$(INSTALL) -D man/tuxcast-config.1 $(MANDIR)
	$(MAKE) -C po install
	$(MAKE) -C hooks install
	
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/tuxcast $(DESTDIR)$(PREFIX)/bin/tuxcast-config #$(DESTDIR)$(PREFIX)/bin/tuxcast-amarok
	rm -rf $(DESTDIR)$(PREFIX)/share/doc/$(APPNAME)

# TODO: Uninstall translation/po stuff?

