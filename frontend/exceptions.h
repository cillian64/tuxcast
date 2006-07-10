#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <iostream>
#include "../libraries/socket_exceptions.h"

using namespace std;

class eNotConnected : public eException
{
	public:
	virtual void print(void)
	{ cout << "Not yet connected" << endl; }
};


#endif
