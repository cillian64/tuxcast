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
		{ cout << "Cannot open socket" << endl; }
};
class eSocket_CannotConnect : public eException
{
	public:
		virtual void print(void)
		{ cout << "Cannot connect" << endl; }
};

class eSocket_AlreadyConnected : public eException
{
	public:
		virtual void print(void)
		{ cout << "Socket already connected" << endl; }
};

class eSocket_CannotResolve : public eException
{
	public:
		virtual void print(void)
		{ cout << "Cannot resolve hostname" << endl; }
};

class eSocket_CannotBind : public eException
{
	public:
		virtual void print(void)
		{ cout << "Cannot bind to port" << endl;}
};

class eSocket_CannotListen : public eException
{
	public:
		virtual void print(void)
		{ cout << "Cannot listen on port" << endl; }
};

class eSocket_CannotAccept : public eException
{
	public:
		virtual void print(void)
		{ cout << "Cannot accept connection" << endl; }
};

#endif
