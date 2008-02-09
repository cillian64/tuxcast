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

#include <iostream>
#include "amarok-plugin.h"

using namespace std;

int main(int argc, char **argv)
{
	cout << "Tuxcast->amarok configuration utility" << endl;
	cout << "Version 0.01a (Tuxcast 0.2)" << endl;
	cout << endl;
	collection amarokdb;
	amarokdb.open("/home/david/.kde/share/apps/amarok/collection.db");
	if(argc == 1)
	{
		cerr << "Invalid or no option specified (a | d | e)" << endl;
		return -1;
	}
	// Could switch :/
	if(argv[1][0] == 'a') // Add feed
		amarokdb.add_podcast("LUGRadio","http://chin.lugradio.org","/home/david/podcasts/LUGRadio");
	if(argv[1][0] == 'd') // Delete feed
		amarokdb.del_podcast("LUGRadio");
	if(argv[1][0] == 'e') // Add episode
		amarokdb.add_episode("http://chin.lugradio.org/ep1.ogg","","http://chin.lugradio.org", "ChinRadio Episode 1");


	return 0;
}

