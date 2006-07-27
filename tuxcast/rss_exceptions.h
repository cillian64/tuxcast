#ifndef RSS_EXCEPTIONS_H
#define RSS_EXCEPTIONS_H

#include "../exceptions.h" // For vanilla
#include <iostream>

using namespace std;

class eRSS_CannotParseFeed : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Cannot parse the RSS feed" << endl; }
};


#endif
