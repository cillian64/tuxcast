#include "filestuff.h"
#include "sys/stat.h"
#include "unistd.h"
#include "../compile_flags.h"
#include "filestuff_exceptions.h"


bool init(void)
{
#ifdef POSIX
	return true;
#endif
#ifdef BSD
	return true;
#endif
#ifdef BOOST
	throw eFilestuff_NoBoost();
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
			throw eFilestuff_CannotCreateFolder();
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
		throw eFilestuff_IsAFile(folder);
#endif
#ifdef BSD
	throw eFilestuff_NoBSD();
	// No BSD yet
	// FIXME
#endif
#ifdef BOOST
	throw eFilestuff_NoBoost();
	// No boost support in here atm.
	// FIXME
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
		return false; // Not throwing an exception - 
		// Returning false here is teh right thing to do,
		// Not an error - we're just saying the file doesn't exist
	}
	else
	{
		// Ok, it exists, let's check it's a file:
		if(S_ISREG(mystat.st_mode))
			return true; // It's a file, we're fine
		else
			throw eFilestuff_NotAFile(file); // Not a file
		// Oops
	}
#endif
#ifdef BSD
	throw eFilestuff_NoBSD();
	// No BSD yet
#endif
#ifdef BOOST
	throw eFilestuff_NoBoost();
	// No boost here yet
#endif
}
