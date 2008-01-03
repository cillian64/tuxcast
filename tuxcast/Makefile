include ../config.mk

TUXCAST_SOURCES=\
	tuxcast.cpp \
	rss.cpp \
	cleaner.cpp \
	../config/config.cpp \
	../libraries/filestuff.cpp \
	tuxcast_functions.cpp \
	../version.cpp
TUXCAST_OBJS=\
	$(TUXCAST_SOURCES:.cpp=.o)

.PHONY: main clean

main: tuxcast

tuxcast: $(TUXCAST_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBXML2_LIBS) $(LIBCURL_LIBS) $(PCREPP_LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ $(LIBXML2_CFLAGS) $(LIBCURL_CFLAGS) $(PCREPP_CFLAGS)

../config/config.o:	
	$(MAKE) -f ../config/Makefile -C ../config

../libraries/filestuff.o: ../libraries/filestuff.cpp
	$(MAKE) filestuff.o -C ../libraries/

../plugins/amarok-plugin.o: ../plugins/amarok-plugin.cpp
	$(MAKE) amarok-plugin.o -C ../plugins/

../version.o: ../version.cpp
	$(MAKE) version.o -C ..

clean:
	rm -f *.o tuxcast
	$(MAKE) -f ../config/Makefile -C ../config clean
