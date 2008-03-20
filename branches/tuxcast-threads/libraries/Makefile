include ../config.mk

main: filestuff.o $(TORRENT_OBJECTS)
	$(TORRENT_BUILD)

filestuff.o: filestuff.cpp
	$(CXX) $(CXXFLAGS) -c $^

clean:
	rm -f filestuff.o
	$(TORRENT_CLEAN)
