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



#ifndef CONFIG_EXCEPTIONS_H
#define CONFIG_EXCEPTIONS_H

#include "../exceptions.h" // For vanilla
#include <stdio.h>
#include <libintl.h>
#include <locale.h>

#define _(x) gettext(x)

class eConfig_NoConfigFile : public eException
{
	public:
		virtual void print(void)
		{ fprintf(stderr,_("No config file found.  (This isn't always fatal)\n")); }
};

class eConfig_CannotSaveConfig : public eException
{
	public:
		virtual void print(void)
		{ fprintf(stderr,_("Couldn't save your config, for some reason\n")); }
};



#endif
