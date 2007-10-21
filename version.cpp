#include <iostream>
#include <libintl.h>
#include <locale.h>

#include "version.h"

using namespace std;

#define _(x) gettext(x)

void version()
{
	printf(_("Tuxcast version %s, Copyright (c) 2006-2007 David Turner\n"));
#ifdef RELEASE_BINARY
	printf(_("Release binary %s\n"));
#endif
#ifdef RELEASE_SOURCE
	printf(_("Release source tarball %s\n"));
#endif
#ifdef SUBVERSION
	printf(_("From subversion, release %s revision %s\n"));
#endif
	printf(_("Tuxcast comes with ABSOLUTELY NO WARANTY; for details see COPYING.\n"));
	printf(_("This is free software, you are welcome to redistribute it under certain conditions; see COPYING for more details\n"));
}

