include ../config.mk

main: tuxcast-amarok amarok-plugin.o
	echo Done

tuxcast-amarok: tuxcast-amarok.o amarok-plugin.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lsqlite3
	# strip -s $@

tuxcast-amarok.o: tuxcast-amarok.cpp amarok-plugin.cpp
	$(CXX) $(CXXFLAGS) -c tuxcast-amarok.cpp

amarok-plugin.o: amarok-plugin.cpp
	$(CXX) $(CXXFLAGS) -c amarok-plugin.cpp

clean:
	rm -f *.o tuxcast-amarok
