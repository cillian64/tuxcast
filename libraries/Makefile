main: filestuff.o socket.o
	# No stuff required here:
	# This makefile is only for building libraries
	# The default target shouldn't really be called, but, meh

filestuff.o: filestuff.cpp
	g++ -c -g filestuff.cpp

socket.o: socket.cpp
	g++ -c -g socket.cpp

clean:
	rm filestuff.o socket.o
