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



#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>

using namespace std;

class feed;

class configuration
{
	public:
	
		string podcastdir;
		bool ask; // Though a string is stored in config
			// it's converted to a bool on load/save
		vector<feed *> feeds; // A vector of feed pointers
		
		void save();
		void load();
};

class feed
{
	public:
		string name;
		string address;
		string folder;
};

#endif
