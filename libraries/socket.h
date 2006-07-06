#include <string>
#include <iostream>
#include <netinet/in.h>


using namespace std;

// Thanks to loufoque, from freenode, for showing my how to catch
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

/*class TCPconnecter : public TCPsocket
{
	public:
		TCPconnecter()
		{ FD=0; }
		void connect(string host, int port);
		void disconnect(void);
		void resolve(string host, struct in_addr *ip);
};
*/
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
		
