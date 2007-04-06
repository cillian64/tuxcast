include ../config.mk

main: tuxcast

tuxcast : tuxcast.o rss.o ../config/config.o ../libraries/filestuff.o tuxcast_functions.o ../version.o 
#../plugins/amarok-plugin.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBXML2_LIBS) $(LIBCURL_LIBS) # ../plugins/amarok-plugin.o -lsqlite3

tuxcast.o: tuxcast.cpp
	$(CXX) $(CXXFLAGS) -c $^ $(LIBXML2_CFLAGS) $(LIBCURL_CFLAGS)

tuxcast_functions.o: tuxcast_functions.cpp
	$(CXX) $(CXXFLAGS) -c $^ $(LIBXML2_CFLAGS) $(LIBCURL_CFLAGS)

rss.o: rss.cpp
	$(CXX) $(CXXFLAGS) -c $^ $(LIBXML2_CFLAGS)


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
