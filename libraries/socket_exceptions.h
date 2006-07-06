#ifndef SOCKET_EXCEPTIONS_H
#define SOCKET_EXCEPTIONS_H

class eException
{
	public:
		virtual void print(void)
		{ cout << "Vanilla Exception (This shouldn't happen)" << endl;}

		// Removed all the constructor and variable junk:
		// If the main program wants to see what went wrong, it can
		// just check strerr or errno itself
};

class eCannotOpenSocket : public eException
{
	public:
		virtual void print(void)
		{ cout << "Cannot open socket" << endl; }
};
class eCannotConnect : public eException
{
	public:
		virtual void print(void)
		{ cout << "Cannot connect" << endl; }
};

class eAlreadyConnected : public eException
{
	public:
		virtual void print(void)
		{ cout << "Socket already connected" << endl; }
};

class eCannotResolve : public eException
{
	public:
		virtual void print(void)
		{ cout << "Cannot resolve hostname" << endl; }
};

class eCannotBind : public eException
{
	public:
		virtual void print(void)
		{ cout << "Cannot bind to port" << endl;}
};

class eCannotListen : public eException
{
	public:
		virtual void print(void)
		{ cout << "Cannot listen on port" << endl; }
};

class eCannotAccept : public eException
{
	public:
		virtual void print(void)
		{ cout << "Cannot accept connection" << endl; }
};

#endif
