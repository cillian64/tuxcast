/*
 * 
 * This file is part of Tuxcast, "The linux podcatcher"
 * Copyright (C) 2006-2008 David Turner
 * Copyright (C) 2010 Mathew Cucuzella (kookjr@gmail.com)
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



#include "filestuff.h"
#include "sys/stat.h"
#include "unistd.h"
#include <fcntl.h>
#include <sys/types.h>
#include "../compile_flags.h"
#include "filestuff_exceptions.h"
#include <string.h>

#include <sstream>

#define LOCKDIR1 "/var/lock"
#define LOCKDIR2 "/tmp"

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

bool move(string oldname, string newname)
{
#ifdef POSIX
	if((strcmp(oldname.c_str(),"") == 0) || (strcmp(newname.c_str(),"") == 0))
		return false;
	if(rename(oldname.c_str(),newname.c_str()) == 0)
		return true;
	else
		return false;
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

/*
 * Try /var/locks first, but fall back to /tmp
 */
static std::string mk_lock_file_name(void) {
    ostringstream path;

    if (access(LOCKDIR1, W_OK) == 0) {
        path << LOCKDIR1;
    }
    else {
        path << LOCKDIR2;  // /tmp is always around
    }

    // lock file name must be user specific
    path << "/tuxcast." << (long )geteuid() << ".lck";

    return path.str();
}

/*
 * Limit execution to one process per system. Note the lock is
 * automatically released when lock_fd is closed, which happens
 * when this process exits.
 */
void set_lock(void) {
    static int lock_fd = -1;
    struct flock fl;

    if (lock_fd != -1) {
        return;
    }

    std::string lock_file = mk_lock_file_name();

    fl.l_type   = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start  = 0;
    fl.l_len    = 1;
    fl.l_pid    = 0;

    if ( (lock_fd=open(lock_file.c_str(), O_WRONLY|O_CREAT, 0666)) == -1 ) {
        throw eProcessLock("Process lock failed, could not open " + lock_file);
    }

    if (fcntl(lock_fd, F_SETLK, &fl) == -1) {
        std::string msg("Process lock failed, fcntl failed");
        if (fcntl(lock_fd, F_GETLK, &fl) != -1) {
            ostringstream newmsg;
            newmsg << "Another process has lock: PID " << fl.l_pid;
            msg = newmsg.str();
        }
        close(lock_fd);
        lock_fd = -1;
        throw eProcessLock(msg);
    }

    return;
}
