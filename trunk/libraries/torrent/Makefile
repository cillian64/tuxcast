include ../../config.mk

main: torrent.o curl_stack.o curl_get.o
	# No stuff required here:
	# This makefile is only for building libraries
	# The default target shouldn't really be called, but, meh

torrent.o: torrent.cpp
	$(CXX) $(CXXFLAGS) -c $^ $(LIBTORRENT_CFLAGS) $(LIBCURL_CFLAGS)

curl_stack.o: curl_stack.cpp
	$(CXX) $(CXXFLAGS) -c $^ $(LIBTORRENT_CFLAGS) $(LIBCURL_CFLAGS)

curl_get.o: curl_get.cpp
	$(CXX) $(CXXFLAGS) -c $^ $(LIBTORRENT_CFLAGS) $(LIBCURL_CFLAGS)

clean:
	rm -f *.o
