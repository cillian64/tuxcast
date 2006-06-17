#include "filestuff.h"
#include "sys/stat.h"
#include "unistd.h"
#include "../compile_flags.h"


bool init(void)
{
#ifdef POSIX
	return true;
#endif
#ifdef BSD
	return true;
#endif
#ifdef BOOST
	return false;
	// ATM boost isn't here yet
	// We'll do the default_name_check init in here in future
	// FIXME
#endif
}

bool checkfolderexists(string folder)
{
#ifdef POSIX
	// Make some structures, etc:
	struct stat mystat;

	// Try to fill the attribute structure
	if(stat(folder.c_str(), &mystat) == -1)
	{
		// We'll assume the error is because it doesn't exist
		// FIXME
		// Let's create the folder:
		if(mkdir(folder.c_str(), 0777) == -1)
		{
			return false;
		}
		else
			return true;
	}
	
	// It already exists: let's check it's type:
	if(S_ISDIR(mystat.st_mode))
		// Already is a dir
		return true;
	else
		// Uh oh, exists but isn't a dir
		// Instead of trying to delete or anything, abort!
	{
		return false;
	}
#endif
#ifdef BSD
	return false;
	// No BSD yet
	// FIXME
#endif
#ifdef BOOST
	return false;
	// Boost is in the main program atm,
	// not here
#endif
}

bool checkfileexists(string file)
{
#ifdef POSIX
	// Define our structures:
	struct stat mystat;
	if(stat(file.c_str(), &mystat) == -1)
	{
		// We'll assume the error is because it doesn't exist
		// FIXME
		return false;
	}
	else
	{
		// Ok, it exists, let's check it's a file:
		if(S_ISREG(mystat.st_mode))
			return true; // It's a file, we're fine
		else
			return false; // Isn't a file, oops
	}
#endif
#ifdef BSD
	return false;
	// No BSD yet
#endif
#ifdef BOOST
	return false;
	// No boost here yet
#endif
}
