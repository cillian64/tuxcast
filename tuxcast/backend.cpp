#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>

using namespace std; // Watch out:
// This must be before tuxcast.h, as it uses strings

#include "backend.h"
#include "tuxcast.h"
#include "../config/config.h" // just incase
#include "rss.h"

// Request types:
// (TODO - convert this to an enum)
#define INFO 0
#define GET 1

#define PROTOCOL_VERSION "0.2"


void OutputError(string error);
// Function to output a nice error tree

void OutputInfo(void);

void backend(void)
{
	xmlDocPtr doc=NULL;
	xmlNodePtr root=NULL, curr=NULL;
	int request_type;
	configuration myconfig;
	myconfig.load();
	string name="";
	int id=0;
	filelist *myfilelist;


	// We ***MUST*** output all status messages or errors to stderr.
	// Anything going to STDOUT *WILL* be interpreted as XML, and could
	// screw up the interfacing program.
	cerr << "Backend mode" << endl;

	// Let's start parsing out input (the request)
	
	doc = xmlReadFile("-", NULL, 0);
	if(doc == NULL)
	{
		cerr << "ERROR: Cannot parse input" << endl;
		// Don't panic, let's still try and exit gracefully:
		OutputError("Invalid Input");
		return;
	}

	root = xmlDocGetRootElement(doc);
	
	if(strcmp((char *)root->name, "tuxcast") != 0)
	{
		cerr << "Invalid input: unknown root element" << endl;
		OutputError("Invalid Input");
		return;
	}

	// Ok, we have our nice, valid input.
	// Let's enter the main parse loop till we find what they want
	// (request->type)
	
	curr = root->children;
	
	while(true)
	{
		if(strcmp((char *)curr->name, "version") == 0)
			cerr << "Protocol version is " << curr->children->content << endl;

		if(strcmp((char *)curr->name, "client") == 0)
			cerr << "Client is " << curr->children->content << endl;

		if(strcmp((char *)curr->name, "request") == 0)
		{
			// Bingo, let's find the request type:
			// Step into request: (we stay in there for the rest of the program)
			curr = curr->children;
			while((strcmp((char *)curr->name, "text") == 0) && curr->next != NULL)
				curr = curr->next;
			if((curr->next == NULL) && (strcmp((char *)curr->name, "text") == 0))
			{
				cerr << "Error parsing tree, no request type" << endl;
				OutputError("No Request Type");
				return;
			}
			
			if(strcmp((char*)curr->children->content, "Info") == 0)
			{
				request_type = INFO;
				break; // Out of while loop
			}
			if(strcmp((char *)curr->children->content, "Get") == 0)
			{
				request_type = GET;
				break;
			}

			// Handle other request types here...
		}
		if(curr->next == NULL)
		{
			cerr << "Hit the end of the input before finding a request" << endl;
			OutputError("No Request Found");
			return;
		}
		else
			curr = curr->next;
	}


	switch(request_type)
	{
		case INFO:
			OutputInfo();		
			break;
/* Not finished yet
		case GET:
			// Get a single feed...
			// First let's parse out the feed name:
			// 
			while((strcmp((char *)curr->name, "text") == 0) && (curr->next != NULL))
				// Loop till we're a node, or hit the end of request
				curr = curr->next;

			if(curr->next = NULL)
			{
				cerr << "Oops, hit the end of request, still no feed name" << endl;
				OutputError("No Feed Name");
				return;
			}

			if(strcmp((char *)curr->name, "Name") != 0)
			{
				cerr << "Eh? Next node in request isn't a feed name!?" << endl;
				OutputError("No Feed Name");
				return;
			}

			// Ok, We need to loop through the config to find this feed's ID
			// Then we pass this to check() which does all the hard work.
			// 
			// Feed name *should* be:
			name = curr->children->content;

			// Loop through myconfig.feeds:
			for(int i=0; i<myconfig.feeds.size(); i++)
				if(strcmp(name.c_str(), myconfig.feeds[i]->name) == 0)
					// Found it:
					// Store the ID
					id = i;
			
			// Check the feed:
			check(&myconfig, id);

			

			
				
		*/		
			

		default:
			cerr << "ERROR: Unhandled request_type" << endl;
			OutputError("Unhandled Request Type");
			return;
	}
				
}

void OutputError(string error)
{
	xmlDocPtr doc=NULL;
	xmlNodePtr root=NULL, curr=NULL;
	
	cerr << "Outputting hardcoded XML tree: Invalid Input" << endl;

	doc = xmlNewDoc((xmlChar *)"1.0");
	root = xmlNewNode(NULL, (xmlChar *)"tuxcast");
	xmlDocSetRootElement(doc, root);

	xmlNewChild(root, NULL, (xmlChar *)"version", (xmlChar *)PROTOCOL_VERSION);
	xmlNewChild(root, NULL, (xmlChar *)"server", (xmlChar *)"Tuxcast Backend");
	xmlNewChild(root, NULL, (xmlChar *)"error", (xmlChar *)error.c_str());

	xmlSaveFormatFileEnc("-", doc, "UTF-8", 1);

	xmlFreeDoc(doc);
	xmlCleanupParser();
}

void OutputInfo(void)
{
	xmlDocPtr doc=NULL;
	xmlNodePtr root=NULL, curr=NULL;

	cerr << "Outputting Info XML tree" << endl;

	doc = xmlNewDoc((xmlChar *)"1.0");
	root = xmlNewNode(NULL, (xmlChar *)"tuxcast");
	xmlDocSetRootElement(doc, root);

	xmlNewChild(root, NULL, (xmlChar *)"version", (xmlChar *)PROTOCOL_VERSION);
	xmlNewChild(root, NULL, (xmlChar *)"server", (xmlChar *)"Tuxcast Backend");

	xmlSaveFormatFileEnc("-", doc, "UTF-8", 1);

	xmlFreeDoc(doc);
	xmlCleanupParser();

}
