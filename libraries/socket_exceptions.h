#ifndef SOCKET_EXCEPTIONS_H
#define SOCKET_EXCEPTIONS_H

#include <iostream>
#include "../exceptions.h"
using namespace std;

// The vanilla exception is now global to everything
// So it's now in ../exceptions.h

class eSocket_CannotOpenSocket : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Cannot open socket" << endl; }
};
class eSocket_CannotConnect : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Cannot connect" << endl; }
};

class eSocket_AlreadyConnected : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Socket already connected" << endl; }
};

class eSocket_CannotResolve : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Cannot resolve hostname" << endl; }
};

class eSocket_CannotBind : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Cannot bind to port" << endl;}
};

class eSocket_CannotListen : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Cannot listen on port" << endl; }
};

class eSocket_CannotAccept : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Cannot accept connection" << endl; }
};

#endif
