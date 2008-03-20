# This is the configuration file for all of the Makefiles.
# 
# The Solaris section assumes that gnu tools from sunfreeware.org 
# or blastwave.org are installed and in the path.

ifeq ($(OSTYPE),solaris)

CXX=g++
OPTIM?=-O2
DEBUG?=-g
CXXFLAGS=$(DEBUG) $(OPTIM)
INSTALL=ginstall
LDFLAGS=-lintl
MAKE=gmake
PREFIX=/usr/local
STRIP=gstrip

XGETTEXT=gxgettext
MSGMERGE=gmsgmerge
MSGFMT=gmsgfmt

LIBXML2_CFLAGS:=$(shell xml2-config --cflags)
LIBXML2_LIBS:=$(shell xml2-config --libs)
LIBCURL_CFLAGS:=$(shell curl-config --cflags)
LIBCURL_LIBS:=$(shell curl-config --libs)
#PCREPP_CFLAGS:=$(shell pcre-config --cflags) $(shell pcre++-config --cflags)
#PCREPP_LIBS:=$(shell pcre-config --libs) $(shell pcre++-config --libs)
LIBTORRENT_CFLAGS:=$(shell pkg-config --cflags sigc++-2.0 libtorrent)
LIBTORRENT_LIBS:=$(shell pkg-config --libs sigc++-2.0 libtorrent)
PTHREAD_LIBS:=-lpthread

TORRENT_BUILD=make -C torrent
TORRENT_CLEAN=make -C torrent clean
TORRENT_LINK=-L../libraries/torrent/torrent.o -L../libraries/torrent/curl_stack.o -L../libraries/torrent/curl_get.o
TORRENT_OBJECTS=torrent/torrent.o torrent/curl_stack.o torrent/curl_get.o
AB_TORRENT_OBJECTS=../libraries/torrent/torrent.o ../libraries/torrent/curl_stack.o ../libraries/torrent/curl_get.o

MANDIR=/usr/share/man/man1/

else

CXX=g++
OPTIM?=-O2
DEBUG?=-g
CXXFLAGS=$(DEBUG) $(OPTIM)
INSTALL=install
LDFLAGS=
MAKE=make
PREFIX=/usr
STRIP=strip

XGETTEXT=xgettext
MSGMERGE=msgmerge
MSGFMT=msgfmt

LIBXML2_CFLAGS:=$(shell xml2-config --cflags)
LIBXML2_LIBS:=$(shell xml2-config --libs)
LIBCURL_CFLAGS:=$(shell curl-config --cflags)
LIBCURL_LIBS:=$(shell curl-config --libs)
#PCREPP_CFLAGS:=$(shell pcre-config --cflags) $(shell pcre++-config --cflags)
#PCREPP_LIBS:=$(shell pcre-config --libs) $(shell pcre++-config --libs)
LIBTORRENT_CFLAGS:=$(shell pkg-config --cflags sigc++-2.0 libtorrent)
LIBTORRENT_LIBS:=$(shell pkg-config --libs sigc++-2.0 libtorrent)
PTHREAD_LIBS:=-lpthread

TORRENT_BUILD=make -C torrent
TORRENT_CLEAN=make -C torrent clean
TORRENT_LINK=-L../libraries/torrent/torrent.o -L../libraries/torrent/curl_stack.o -L../libraries/torrent/curl_get.o
TORRENT_OBJECTS=torrent/torrent.o torrent/curl_stack.o torrent/curl_get.o
AB_TORRENT_OBJECTS=../libraries/torrent/torrent.o ../libraries/torrent/curl_stack.o ../libraries/torrent/curl_get.o

MANDIR=/usr/share/man/man1/

endif

