#!/bin/bash

OK=1

if xml2-config --cflags 2>&1 >/dev/null
then
echo libxml: Looks good
else
echo libxml: Not so good
OK=0
fi

if curl-config --cflags 2>&1 >/dev/null
then
echo libcurl: Looks good
else
echo libcurl: Not so good
OK=0
fi

if pkg-config --cflags sigc++-2.0 2>&1 >/dev/null
then
echo sigc++: Looks good
else
echo sigc++: Not so good
OK=0
fi

if pkg-config --cflags libtorrent 2>&1 >/dev/null
then
echo libtorrent: Looks good
else
echo libtorrent: Not so good
OK=0
fi
echo ---
if test $OK -eq 1
then
echo Everything looks good
else
echo One or more libraries are missing
echo Note: This may still work if you aren\'t
echo compiling all features.
fi

