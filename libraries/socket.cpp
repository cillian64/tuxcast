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

	
	
