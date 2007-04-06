include config.mk

main:
	$(MAKE) -C tuxcast
	#$(MAKE) -C plugins
	$(MAKE) -C po

clean:
	$(MAKE) -C tuxcast clean
	$(MAKE) -C libraries clean
	#$(MAKE) -C plugins clean
	$(MAKE) -C po clean

version.o: version.cpp
	$(CXX) $(CXXFLAGS) -c version.cpp

install: main
	$(STRIP) -s tuxcast/tuxcast
	$(STRIP) -s config/tuxcast-config
	$(INSTALL) -D -m 755 tuxcast/tuxcast $(DESTDIR)$(PREFIX)/bin/tuxcast
	$(INSTALL) -D -m 755 config/tuxcast-config $(DESTDIR)$(PREFIX)/bin/tuxcast-config
	#$(INSTALL) -m 755 plugins/tuxcast-amarok $(DESTDIR)$(PREFIX)/bin/tuxcast-amarok
	$(MAKE) -C po install
	

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/tuxcast $(DESTDIR)$(PREFIX)/bin/tuxcast-config #$(DESTDIR)$(PREFIX)/bin/tuxcast-amarok


