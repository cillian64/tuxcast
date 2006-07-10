#include "backend.h"

backend::backend(void)
{
	server="localhost";
	port=1759;
}

backend::backend(string server)
{
	this->server=server.c_str();
	port=1759;
}

backend::backend(string server, int port)
{
	this->server=server;
	this->port=port;
}

void backend::connect(void)
{
	connection.connect(server, port);
}

void backend::disconnect(void)
{
	connection.disconnect();
}

void backend::check(string feed)
{
	string output;
	// Lets generate some output:
	output="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<tuxcast>\n";
	output=output+"<version>0.2</version>\n<client>Tuxcast-frontend";
	output=output+"</client>\n<request>\n><type>Check</type>\n<feed>";
	output=output+feed+"</feed>\n</request>\n</tuxcast>";
	write(this->connection.getFD(), output.c_str(), output.size());
}

void backend::up2date(string feed)
{
	string output;
	// Lets generate some output:
	output="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<tuxcast>\n";
	output=output+"<version>0.2</version>\n<client>Tuxcast-frontend";
	output=output+"</client>\n<request>\n><type>Up2date</type>\n<feed>";
	output=output+feed+"</feed>\n</request>\n</tuxcast>";
	write(this->connection.getFD(), output.c_str(), output.size());
}
