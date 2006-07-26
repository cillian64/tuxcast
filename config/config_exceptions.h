#ifndef CONFIG_EXCEPTIONS_H
#define CONFIG_EXCEPTIONS_H

#include <iostream>
#include "../exceptions.h" // For vanilla

using namespace std;

class eConfig_NoConfigFile : public eException
{
	public:
		virtual void print(void)
		{ cout << "No config file found.  (This isn't always fatal)" << endl; }
};



#endif
