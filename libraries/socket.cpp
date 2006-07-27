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



#include "socket.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include "socket_exceptions.h"


using namespace std;

void TCPlistener::listen(int port)
{
	int sock;
	struct sockaddr_in localaddr;
	struct sockaddr_in remoteaddr;

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw eSocket_CannotOpenSocket();
	
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(port);
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sock, (struct sockaddr *)&localaddr, sizeof(struct sockaddr_in)) == -1)
		throw eSocket_CannotBind();

	if(::listen(sock, 0) == -1)
		throw eSocket_CannotListen();

	FD=sock;
}

void TCPlistener::getconnection(TCPconnection *connection)
{
	struct sockaddr_in remoteaddr;
	socklen_t len;
	len = sizeof(sockaddr_in);
	connection->FD = accept(this->FD, (struct sockaddr *)&remoteaddr, &len);
	memcpy(&connection->remoteaddr, &remoteaddr, sizeof(remoteaddr));
}

void TCPconnecter::connect(string host, int port)
{
	int sock;
	struct sockaddr_in server;

	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		throw eSocket_CannotOpenSocket();

	resolve(host);

	server.sin_family = AF_INET;
	server.sin_port = htons(port); // Remember the byte order!
	server.sin_addr.s_addr = remoteaddr.s_addr; // Use the IP we just
	// resolved
	
	cout << inet_ntoa(server.sin_addr) << endl;

	if(::connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
		throw eSocket_CannotConnect();

	FD = sock;
}

void TCPconnecter::disconnect(void)
{
	close(FD);
}

void TCPconnecter::resolve(string host)
{
	struct hostent *theirhost;
	
	if((theirhost = gethostbyname(host.c_str())) == NULL)
		throw eSocket_CannotResolve();

	remoteaddr.s_addr = *((unsigned long *)theirhost->h_addr_list[0]);
	
}

	
	
