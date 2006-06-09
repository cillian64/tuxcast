#ifndef FILESTUFF_H
#define FILESTUFF_H

#include <string>

using namespace std;

bool init(void);
// If using boost, it does the default_name_check init stuff
// With POSIX or BSD< it just returns true atm.

bool checkfolderexists(string folder);
// If the folder doesn't exist, make it, if it does exist, leave it
// If the folder exists or was made, return true, if error, false

bool checkfileexists(string file);
// If the file doesn't exist, return false
// If it does exist, true

#endif
