class eException
{
	public:
		eException (int err=0)
		{ error = err; }
		virtual void print(void)
		{ cout << "Vanilla Exception (This shouldn't happen)" << endl;
		if(error) cout << "Errno is " << sys_errlist[error] << endl; }

	protected:
		int error;
};

class eCannotOpenSocket : public eException
{
	public:
		eCannotOpenSocket(int err=0)
		{ this->error = err; }
		virtual void print(void)
		{ cout << "Cannot open socket" << endl; 
		if(error) cout << "Errno is " << sys_errlist[error] << endl; }
};
class eCannotConnect : public eException
{
	public:
		eCannotConnect(int err=0)
		{ this->error = err; }
		virtual void print(void)
		{ cout << "Cannot connect" << endl; 
		if(error) cout << "Errno is " << sys_errlist[error] << endl; }
};

class eAlreadyConnected : public eException
{
	public:
		eAlreadyConnected(int err=0)
		{ this->error = err; }
		virtual void print(void)
		{ cout << "Socket already connected" << endl;
		if(error) cout << "Errno is " << sys_errlist[error] << endl;}
};

class eCannotResolve : public eException
{
	public:
		eCannotResolve(int err=0)
		{ this->error = err; }
		virtual void print(void)
		{ cout << "Cannot resolve hostname" << endl;
		if(error) cout << "Errno is " << sys_errlist[error] << endl;}
};

class eCannotBind : public eException
{
	public:
		eCannotBind(int err=0)
		{ this->error = err; }
		virtual void print(void)
		{ cout << "Cannot bind to port" << endl;
		if(error) cout << "Errno is " << sys_errlist[error] << endl;}
};

class eCannotListen : public eException
{
	public:
		eCannotListen(int err=0)
		{ this->error = err; }
		virtual void print(void)
		{ cout << "Cannot listen on port" << endl;
		if(error) cout << "Errno is " << sys_errlist[error] << endl;}
};

class eCannotAccept : public eException
{
	public:
		eCannotAccept(int err=0)
		{ this->error = err; }
		virtual void print(void)
		{ cout << "Cannot accept connection" << endl;
		if(error) cout << "Errno is " << sys_errlist[error] << endl;}
};
