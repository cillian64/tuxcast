/*
 * 
 * This file is part of Tuxcast, "The linux podcatcher"
 * Copyright (C) 2006-2007 David Turner
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



#ifndef FILESTUFF_EXCEPTIONS_H
#define FILESTUFF_EXCEPTIONS_H

#include "../exceptions.h"
#include <iostream>
#include "../compile_flags.h"

using namespace std;

class eFilestuff_CannotInit : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Couldn't init (Are you trying to use boost...?" << endl; }
};

class eFilestuff_CannotCreateFolder : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Couldn't create folder." << endl; }
};

class eFilestuff_IsAFile : public eFilestuff_CannotCreateFolder // This is just a special case of not being able to create the folder
{
	public:
		eFilestuff_IsAFile(string name)
		{ this->name = name; }
		virtual void print(void)
		{ cerr << "Couldn't create folder \"" << name << "\".  It already exists but isn't of type, directory.  Please delete it and try again" << endl; }
	private:
		string name; // Printing the name here is more important than
		// other places - the user needs to know which file to delete
};

#ifdef BOOST
class eFilestuff_NoBoost : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Sorry, boost isn't implemented yet.  Please recompile with POSIX enabled in compile_flags.h, or contact your package maintainer" << endl; }
};
#endif

#ifdef BSD
class eFilestuff_NoBoost : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Sorry, BSD isn't implemented yet.  Please recompile with POSIX enabled in compile_flags.h, or contact your package maintainer" << endl; }
};
#endif

class eFilestuff_NotAFile : public eException
{
	public:
		eFilestuff_NotAFile(string name)
		{ this->name = name; }
		virtual void print(void)
		{ cerr << "\"" << name << "\" exists, but isn't a file.  Please delete it and try again" << endl; }
	private:
		string name;
};

#endif // End of inclusion guard
