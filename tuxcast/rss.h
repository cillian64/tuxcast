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



#include <string>
#include <iostream>
#include <vector>
#include <libxml/tree.h> // Don't need parser in here, hopefully inclusion
// guard should stop double-inclusion in rss.cpp buggering up.

using namespace std;

class file
{
	public:
		string filename;
		string URL;
		unsigned long length;
};

class filelist
{
	public:

		// Just opening these up to the public eliminates all that
		// messing about with accessor functions, etc....
		vector<file *> files; // Note - it's a vector of FILE POINTERS!!!
		// This means you'll have to access it like files[123]->...
		// instead of files[123].xyz
		
		// Length is replaced by files.size()
};

filelist *parse(string feed);
void addtolist(filelist *myfilelist, xmlNode *enclosure);
