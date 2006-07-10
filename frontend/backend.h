#ifndef BACKEND_H
#define BACKEND_H

#include <string>
#include "exceptions.h"
#include "../libraries/socket.h"

using namespace std;

class backend
{
	public:
		// Constructors:
		backend(string server, int port);
		backend(void);
		backend(string server);

		// Connection management
		void connect(void);
		void disconnect(void);

		// Actions:
		void check(string feed);
		void up2date(string feed);

		// Config related stuff:
	
	private:
		string server;
		int port;
		TCPconnecter connection;
};


#endif
