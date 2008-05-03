include ../config.mk

TUXCAST_SOURCES=\
	tuxcast.cpp \
	rss.cpp \
	cleaner.cpp \
	../config/config.cpp \
	../libraries/filestuff.cpp \
	../libraries/bits.cpp \
	tuxcast_functions.cpp \
	../version.cpp
TUXCAST_OBJS=\
	$(TUXCAST_SOURCES:.cpp=.o)

.PHONY: main clean

main: tuxcast

tuxcast: $(TUXCAST_OBJS) $(AB_TORRENT_OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBXML2_LIBS) $(LIBCURL_LIBS) $(TORRENT_LINK) $(LIBTORRENT_LIBS) $(PTHREAD_LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ $(LIBXML2_CFLAGS) $(LIBCURL_CFLAGS) 

../config/config.o:	
	$(MAKE) -f ../config/Makefile -C ../config

../libraries/filestuff.o: ../libraries/filestuff.cpp
	$(MAKE) filestuff.o -C ../libraries/

../libraries/bits.o: ../libraries/bits.cpp
	$(MAKE) bits.o -C ../libraries/

../libraries/torrent/torrent.o: ../libraries/torrent/torrent.cpp
	$(MAKE) torrent.o -C ../libraries/torrent

../libraries/torrent/curl_get.o: ../libraries/torrent/curl_get.cpp
	$(MAKE) curl_get.o -C ../libraries/torrent

../libraries/torrent/curl_stack.o: ../libraries/torrent/curl_get.cpp
	$(MAKE) curl_stack.o -C ../libraries/torrent

../plugins/amarok-plugin.o: ../plugins/amarok-plugin.cpp
	$(MAKE) amarok-plugin.o -C ../plugins/

../version.o: ../version.cpp
	$(MAKE) version.o -C ..

clean:
	rm -f *.o tuxcast
	$(MAKE) -f ../config/Makefile -C ../config clean
