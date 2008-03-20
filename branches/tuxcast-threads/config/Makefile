include ../config.mk

main: tuxcast-config

tuxcast-config : config.o tuxcast-config.o ../libraries/filestuff.o ../version.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBXML2_LIBS)

clean:
	rm -f *.o
	rm -f tuxcast-config

config.o: config.cpp
	$(CXX) $(CXXFLAGS) -c $^ $(LIBXML2_CFLAGS)

tuxcast-config.o: tuxcast-config.cpp
	$(CXX) $(CXXFLAGS) -c $^

../libraries/filestuff.o: ../libraries/filestuff.cpp
	$(MAKE) filestuff.o -C ../libraries/

../version.o: ../version.cpp
	$(MAKE) version.o -C ..
