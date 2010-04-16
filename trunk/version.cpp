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

#include <stdio.h>
#include <libintl.h>
#include <locale.h>

#include "version.h"
#include "compile_flags.h"

using namespace std;

#define _(x) gettext(x)

void version()
{
	printf(_("Tuxcast version %s, Copyright (c) 2006-2008 David Turner\n"), RELEASE_VERSION);
#ifdef RELEASE_BINARY
	printf(_("Release binary %s\n"), RELEASE_VERSION);
#endif
#ifdef RELEASE_SOURCE
	printf(_("Release source tarball %s\n"), RELEASE_VERSION);
#endif
#ifdef SUBVERSION
#ifdef SUBVERSION_BRANCH
	printf(_("From subversion branch %s, release %s revision %s\n"), SUBVERSION_BRANCH, RELEASE_VERSION, SUBVERSION_REVISION);
#else
	printf(_("From subversion trunk, release %s revision %s\n"), RELEASE_VERSION, SUBVERSION_REVISION);
#endif
#endif
#ifdef POSIX
	printf(_("Compiled for POSIX\n"));
#endif
#ifdef BOOST
	printf(_("Compiled for boost\n"));
#endif
#ifdef BSD
	printf(_("Compiled for BSD\n"));
#endif
#ifdef TORRENT
	printf(_("Compiled with libtorrent bittorrent support\n"));
#endif
	printf(_("Tuxcast comes with ABSOLUTELY NO WARANTY; for details see COPYING.\n"));
	printf(_("This is free software, you are welcome to redistribute it under certain conditions; see COPYING for more details\n"));
}

