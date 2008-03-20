include ../config.mk

POFILES=$(wildcard *.po)
MOFILES=$(POFILES:.po=.mo)
POSOURCES=$(wildcard ../tuxcast/*.cpp) $(wildcard ../libraries/*.cpp) $(wildcard ../config/*.cpp)
POFLAGS=

main: messages mo

.PHONY: main messages mo clean install uninstall

messages: $(POFILES)

mo: $(MOFILES)

tuxcast.pot: $(POSOURCES)
	@echo "updating $@ from sources: $^"
	$(XGETTEXT) -k_ -kN_ $(POFLAGS) $(POSOURCES) -o tmp.po -d tuxcast
	$(MSGMERGE) -U tuxcast.pot tmp.po 2>/dev/null
	-rm -f tmp.po

%.po: tuxcast.pot
	@echo "updating $@ from $<"
	$(MSGMERGE) -U $@ $<

%.mo: %.po
	@echo "compiling $@ from $<"
	$(MSGFMT) -o $@ $<

clean:
	-rm -f $(MOFILES)

install: mo
	@for j in $(MOFILES); do \
		k=`basename $$j`; \
		country=`echo $$j | cut -d "." -f 1`; \
		dir=$(DESTDIR)$(PREFIX)/share/locale/$$country/LC_MESSAGES; \
		echo "installing $$dir/tuxcast.mo ($$k)"; \
		install -d $$dir; \
		cp $$j $$dir/tuxcast.mo; \
	done

uninstall:
	@for j in $(POFILES); do \
		k=`basename $$j`; \
		country=`echo $$k | cut -d "." -f 1`; \
		dir=$(DESTDIR)$(PREFIX)/share/locale/$$country/LC_MESSAGES; \
		rm -f $$dir/tuxcast.mo; \
	done

