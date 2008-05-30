#!/bin/bash


# 
# This file is part of Tuxcast, "The linux podcatcher"
# Copyright (C) 2006-2008 David Turner
# 
# Tuxcast is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
 
# Tuxcast is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Tuxcast; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
# 



echo Tuxcast transcoder hook script. Copyright \(c\) 2006-2008 David Turner
echo Variables are: ${1} ${2}

# TODO: Check MIME type in $2

# TODO: Extra sanity checks ?

# TODO: Handle no . or beginning with .

# TODO: Make mplayer quiet

WORKINGDIR=`dirname ${1}`
FILENAME=`basename ${1}`
STEM=`basename ${FILENAME} .${FILENAME/*./}`

cd ${WORKINGDIR}

mplayer -ao pcm:file=${STEM}.wav ${FILENAME}
oggenc ${STEM}.wav
rm -v ${STEM}.wav
