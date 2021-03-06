/*
 * 
 * This file is part of Tuxcast, "The linux podcatcher"
 * Copyright (C) 2006-2008
 * David Turner
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
#include <sqlite3.h>

using namespace std;
// TODO: Add exceptions...

class collection
{
	public:
		collection();
		void open(string path);
		void add_podcast(string name, string url, string folder); // Add a podcast to collection->podcastchannels
		void del_podcast(string name);
		void add_episode(string url, string localurl, string parent, string title);
		
	private:
		sqlite3 *db;
};

// Callbacks:
static int callback_dud(void *cookie, int argc, char **argv, char **colname);

