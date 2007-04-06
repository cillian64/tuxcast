# This is the configuration file for all of the Makefiles.
# 
# The Solaris section assumes that gnu tools from sunfreeware.org 
# or blastwave.org are installed and in the path.

ifeq ($(OSTYPE),solaris)

CXX=g++
CXXFLAGS=-g -O2
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

else

CXX=g++
CXXFLAGS=-g -O2
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

endif

