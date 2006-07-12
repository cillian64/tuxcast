#include "backend.h"
#include <libxml/parser.h>
#include <libxml/tree.h>

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

void backend::getoutput(backendoutput *output)
{
	string buffer="";
	char buff2[4096];

	xmlDocPtr doc;
	xmlNodePtr curr, curr2, root;

	while(true)
	{
		bzero(buff2, 4096); // Clear out buff2
		read(this->connection.getFD(), buff2, 4096);
		buffer=buffer+buff2;
		if(strstr(buffer.c_str(), "</tuxcast>"))
			break;
	}
	
	// We have the buffer - lets parse!
	doc = xmlReadMemory(buffer.c_str(), buffer.size(), "", "", 0);
	root = xmlDocGetRootElement(doc);

	// Skip over all the info gunk till we get to the episodes
	curr = root->children;
	while(true)
	{
		if(strcmp((char *)curr->name, "output") == 0)
			break;
		if(curr->next==NULL)
			// We're screwed
			// throw eSomething();
			// TODO
			return;
		curr = curr->next;
	}
	curr = curr->children;
	while(curr->children==NULL)
	{
		if(curr->next == NULL)
			return;
		else
			curr=curr->next;
	}
	// We're in feed, down another:
	curr = curr->children;
	// Skip till episode:
	while(true)
	{
		if(strcmp((char *)curr->name, "episode") == 0)
			break;
		if(curr->next == NULL)
			// Uh oh
			// TODO
			return;
		curr = curr->next;
	}
	curr = curr->next;
	// We're at the first episode, loop through them adding each to the output
	while(true)
	{
		if(strcmp((char *)curr->name, "episode") == 0)
		{
			curr2 = curr->children;
			// Loop through the filename, url and status:
			while(true)
			{
				string test;
				if(strcmp((char *)curr2->name, "filename") == 0)
					
					//output->names.push_back((char *)curr2->children->content);
				if(strcmp((char *)curr2->name, "url") == 0)
					output->URLs.push_back((char *)curr2->children->content);
				if(strcmp((char *)curr2->name, "status") == 0)
					output->statuses.push_back((char *)curr2->children->content);

				if(curr2->next == NULL)
					break;
				curr2 = curr2->next;
			}
		}
		if(curr->next == NULL)
			// We're done:
			return;
		curr = curr->next;
	}
}
