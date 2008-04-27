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



#include "../compile_flags.h"
#include "bits.h"

using namespace std;

void replace(string &thestring, map<char, string> &vars)
{
	int pos=-2; // So the first search starts at 0

	// +2 so the char after each % found is skipped
	// So %% works a bit more sanely
	while((pos = thestring.find("%",pos+2)) != -1)
	{
		if(pos == thestring.size()-1)
			// EOF
			break;
		if(thestring[pos+1] == '%')
			// Leave it - %%
			continue;

		thestring.replace(pos,2,vars[thestring[pos+1]]);
		// If the var isn't found, life is good - leave it.
	}
}

