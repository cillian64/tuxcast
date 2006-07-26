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
		{ cout << "Couldn't init (Are you trying to use boost...?" << endl; }
};

class eFilestuff_CannotCreateFolder : public eException
{
	public:
		virtual void print(void)
		{ cout << "Couldn't create folder." << endl; }
};

class eFilestuff_IsAFile : public eFilestuff_CannotCreateFolder // This is just a special case of not being able to create the folder
{
	public:
		eFilestuff_IsAFile(string name)
		{ this->name = name; }
		virtual void print(void)
		{ cout << "Couldn't create folder \"" << name << "\".  It already exists but isn't of type, directory.  Please delete it and try again" << endl; }
	private:
		string name; // Printing the name here is more important than
		// other places - the user needs to know which file to delete
};

#ifdef BOOST
class eFilestuff_NoBoost : public eException
{
	public:
		virtual void print(void)
		{ cout << "Sorry, boost isn't implemented yet.  Please recompile with POSIX enabled in compile_flags.h, or contact your package maintainer" << endl; }
};
#endif

#ifdef BSD
class eFilestuff_NoBoost : public eException
{
	public:
		virtual void print(void)
		{ cout << "Sorry, BSD isn't implemented yet.  Please recompile with POSIX enabled in compile_flags.h, or contact your package maintainer" << endl; }
};
#endif

class eFilestuff_NotAFile : public eException
{
	public:
		eFilestuff_NotAFile(string name)
		{ this->name = name; }
		virtual void print(void)
		{ cout << "\"" << name << "\" exists, but isn't a file.  Please delete it and try again" << endl; }
	private:
		string name;
};

#endif // End of inclusion guard
