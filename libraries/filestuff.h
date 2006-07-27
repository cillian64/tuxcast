/*
 * 
 * This file is part of Tuxcast, "The linux podcatcher"
 * Copyright (C) 2006 David Turner
 * 
 * Tuxcast is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Tuxcast is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Tuxcast; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */



#ifndef FILESTUFF_H
#define FILESTUFF_H

#include <string>

using namespace std;

bool init(void);
// If using boost, it does the default_name_check init stuff
// With POSIX or BSD< it just returns true atm.

bool checkfolderexists(string folder);
// If the folder doesn't exist, make it, if it does exist, leave it
// If the folder exists or was made, return true, if error, false

bool checkfileexists(string file);
// If the file doesn't exist, return false
// If it does exist, true

#endif
