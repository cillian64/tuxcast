/*
 * 
 * This file is part of Tuxcast, "The linux podcatcher"
 * Copyright (C) 2006-2008 David Turner
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

#ifndef RSS_H
#define RSS_H


#include <string>
#include <vector>
#include <memory>

#include <sys/types.h>

using namespace std;

extern const string NS_ATOM;
extern const string NS_RDF;

struct file
{
	string filename;
	string URL;
	string type;
	off_t length;

	file(void)
		: filename()
		, URL()
		, length(0)
		, type()
	{
	}

	file(const string &filename, const string &URL, off_t length, string type)
		: filename(filename)
		, URL(URL)
		, length(length)
		, type(type)
	{
	}
};

typedef vector<struct file> filelist;

auto_ptr<filelist> parse(string feed);

#endif
