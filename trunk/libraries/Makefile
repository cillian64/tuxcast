include ../config.mk

main: filestuff.o socket.o
	# No stuff required here:
	# This makefile is only for building libraries
	# The default target shouldn't really be called, but, meh

filestuff.o: filestuff.cpp
	$(CXX) $(CXXFLAGS) -c $^

clean:
	rm -f filestuff.o
