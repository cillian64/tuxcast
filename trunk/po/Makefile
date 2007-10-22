include ../config.mk

POFILES=*.po
POSOURCES=../tuxcast/*.cpp ../libraries/*.cpp ../config/*.cpp
POFLAGS=

main : messages mo

tuxcast.pot : $(POSOURCES)
	$(XGETTEXT) -k_ -kN_ $(POFLAGS) $(POSOURCES) -o $@
	
messages : tuxcast.pot $(POFILES)
	@for j in $(POFILES); do \
		echo "process $$j"; \
		$(MSGMERGE) $$j tuxcast.pot > .$$j 2>/dev/null; \
		mv -f .$$j $$j; \
	done

mo : $(POFILES)
	@for j in $(POFILES); do \
		echo "converting $$j"; \
		k=`basename $$j`; \
		country=`echo $$k | cut -d "." -f 1`; \
		$(MSGFMT) -o $$country.mo $$j; \
	done

clean:
	-rm -f *.mo

install: messages mo
	@for j in $(POFILES); do \
		k=`basename $$j`; \
		country=`echo $$k | cut -d "." -f 1`; \
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

