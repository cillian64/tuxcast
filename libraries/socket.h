/*
 * 
 * This file is part of Tuxcast, "The linux podcatcher"
 * Copyright (C) 2006 David Turner
 * 
 * Tuxcast is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Tuxcast is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Tuxcast; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */



#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <netinet/in.h>

using namespace std; // Careful to put this before exceptions:
// Some inline IO functions use cout, etc...

#include "socket_exceptions.h"



// Thanks to loufoque, from freenode's ##c++, for showing me how to catch
// these exceptions properly, :)

class TCPconnection
{
	// Used by the listener
	public:
		int FD;
		struct sockaddr_in remoteaddr;
		void close(void)
		{ ::close(this->FD); }
};
		




class TCPsocket
{
	public:
		TCPsocket() // Constructor
		{ FD=0; }
		void write(string data);
		string read(void);
		int getFD(void)
		{ return this->FD; }

	protected:
		int FD;
};

class TCPconnecter : public TCPsocket
{
	public:
		TCPconnecter()
		{ FD=0; }
		void connect(string host, int port);
		void disconnect(void);
	private:
		void resolve(string host);
		struct in_addr remoteaddr;
};


class TCPlistener : public TCPsocket
{
	public:
		TCPlistener()
		{ FD=0; }
		void listen(int port);
		void getconnection(TCPconnection *connection);
		void close(void)
		{ ::close(this->FD); }
};

#endif
