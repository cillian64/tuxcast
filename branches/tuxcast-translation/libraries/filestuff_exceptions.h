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
#include "../compile_flags.h"

#include <libintl.h>
#include <locale.h>

#include <stdio.h>

#define _(x) gettext(x)

using namespace std;

class eFilestuff_CannotInit : public eException
{
	public:
		virtual void print(void)
		{ fprintf(stderr, _("Couldn't initialize the filestuff library, are you trying to use boost? It's not supported!\n")); }
};

class eFilestuff_CannotCreateFolder : public eException
{
	public:
		virtual void print(void)
		{ fprintf(stderr,_("Couldn't create folder.\n")); }
};

class eFilestuff_IsAFile : public eFilestuff_CannotCreateFolder // This is just a special case of not being able to create the folder
{
	public:
		eFilestuff_IsAFile(string name)
		{ this->name = name; }
		virtual void print(void)
		{ fprintf(stderr, _("Couldn't create folder \"%s\".  It already exists but isn't a directory.  Please delete it and try again.\n"), name.c_str()); }
	private:
		string name; // Printing the name here is more important than
		// other places - the user needs to know which file to delete
};

#ifdef BOOST
class eFilestuff_NoBoost : public eException
{
	public:
		virtual void print(void)
		{ fprintf(stderr, _("Sorry, boost isn't implemented yet.  Please recompile with POSIX enabled in compile_flags.h, or contact your package maintainer\n")); }
};
#endif

#ifdef BSD
class eFilestuff_NoBoost : public eException
{
	public:
		virtual void print(void)
		{ fprintf(stderr, _("Sorry, BSD isn't implemented yet.  Please recompile with POSIX enabled in compile_falgs.h, or contact your package maintainer\n")); }
};
#endif

class eFilestuff_NotAFile : public eException
{
	public:
		eFilestuff_NotAFile(string name)
		{ this->name = name; }
		virtual void print(void)
		{ fprintf(stderr, _("\"%s\" exists, but isn't a file.  Please delete it and try again.\n"), name.c_str()); }
	private:
		string name;
};

#endif // End of inclusion guard
