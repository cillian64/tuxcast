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

// This is a bunch of parsers of the tuxcast protocol
#ifndef PROTOCOL_H
#define PROTOCOL_H

// Terms: A set is a combination of a command and it's parameters
// A set it not necessarily a valid command, it might not have enough parameters
// Or the wrong format.
// Line: A line contains all the same stuff as a set, but it's in a single string in the format sent over the net in/

#include <string>
#include <vector>

class set
{
	public:
		string command;
		vector<string> parameters;

		void parse(string line);
};


#endif
