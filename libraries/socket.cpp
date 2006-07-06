#include "socket.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include "socket_exceptions.h"

void TCPlistener::listen(int port)
{
	int sock;
	struct sockaddr_in localaddr;
	struct sockaddr_in remoteaddr;

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw eCannotOpenSocket(errno);
	
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(port);
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sock, (struct sockaddr *)&localaddr, sizeof(struct sockaddr_in)) == -1)
		throw eCannotBind(errno);

	if(::listen(sock, 0) == -1)
		throw eCannotListen(errno);

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
	
	
