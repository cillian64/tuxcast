#include "version.h"
#include <iostream>

using namespace std;

void version()
{
	cout << "Tuxcast version " << RELEASE_VERSION << ", Copyright (c) 2006-2007 David Turner" << endl;
#ifdef RELEASE_BINARY
	cout << "Release binary " << RELEASE_VERSION << endl;
#endif
#ifdef RELEASE_SOURCE
	cout << "Release source tarball " << RELEASE_VERSION << endl;
#endif
#ifdef SUBVERSION
	cout << "From subversion, release " << RELEASE_VERSION << " revision " << SUBVERSION_REVISION << endl;
#endif
	cout << "Tuxcast comes with ABSOLUTELY NO WARANTY; for details see COPYING." << endl;
	cout << "This is free software, you are welcome to redistribute it under certain conditions; see COPYING for more details" << endl;
}

