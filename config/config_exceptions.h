#ifndef CONFIG_EXCEPTIONS_H
#define CONFIG_EXCEPTIONS_H

#include <iostream>
#include "../exceptions.h" // For vanilla

using namespace std;

class eConfig_NoConfigFile : public eException
{
	public:
		virtual void print(void)
		{ cerr << "No config file found.  (This isn't always fatal)" << endl; }
};

class eConfig_CannotSaveConfig : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Couldn't save your config, for some reason" << endl; }
};



#endif
