#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>
#include <curl/curl.h>
#include <errno.h>

using namespace std; // Watch out:
// This must be before tuxcast.h, as it uses strings

#include "backend.h"
#include "tuxcast.h"
#include "../config/config.h" // just incase
#include "rss.h"
#include "../libraries/filestuff.h"
#include "../libraries/socket.h" // SHOULD include the socket exceptions


// Request types:
// (TODO - convert this to an enum)
#define INFO 0
#define CHECK 1

// Unimplemented types:
#define CHECKALL 2
#define UP2DATE 3
#define UP2DATEALL 4

// Random globals:
#define PROTOCOL_VERSION "0.2"
#define PORT 1759

// Function to output a nice error tree:
void OutputError(string error);

// Outputs server info
void OutputInfo();


// XML-outputting replicas of functions in tuxcast.h
// (These load their own config, and search the config file, so they only take a feed name)
// (Backend still uses the newfile/alreadydownled from tuxcast.h)
string xmlget(string filename, string url, string folder); // Returns a status string
void xmlcheck(string name);

// Unimplemented:
void xmlcheckall(void);
void xmlup2date(string name);
void xmlup2dateall(void);

// Some nice internal helper functions
// (Mostly copies of tuxcast.h stuff)
int idfromname(string name);

// *** WARNING, THIS HAS CHANGED: ***
// Now, the XML is parsed in here:
void parsexml1(xmlDocPtr doc);
// The listener loop, all TCP stuff, and the code to load XML from a FD
// is all in backend()
// The parsexml2 function is called by parsexml1
// It goes actually parses and does the request -
// parsexml1 only checks the request type

// Parses the XML for the request type and calls the appropriate function
void parsexml2(int request_type, xmlNodePtr node);

// Yes, I know globals are evil, but it beats having int FD in every single function's arguments
int FD;

void backend(void)
{
	// We run all this in a try so we can catch exceptions
	// and do a nice OutputError.  Currently, only this function
	// does this, as the socket code is the only stuff with exceptions
	try
	{
		TCPlistener socket;
		TCPconnection connection;
		string buffer="";
		char buffer2[4096];
		socket.listen(PORT);

		cerr << "Listening on " << PORT << endl;

		while(true)
		{
			// Clear out the buffers from the last connection:
			buffer="";
			socket.getconnection(&connection);
			cerr << "Got connection!" << endl;
			cerr << "(FD = " << connection.FD << ")" << endl;
			FD = connection.FD;

			// What we do here, is keep reading from teh socket
			// till we get an error, or the XML doc is finished
			// This allows us to have the XML session sent in lots
			// of bits, over a period of time
			while(true)
			{
				if(read(connection.FD, buffer2, 4096) == 0)
					break; // EOF - can't read anymore
				buffer = buffer + buffer2;
				if(strstr(buffer.c_str(), "</tuxcast"))
						break;
				cerr << "Incomplete input, looping..." << endl;
			}
			if(!strstr(buffer.c_str(), "</tuxcast>"))
			{
				cerr << "Incomplete input and EOF" << endl;
				cerr << "Bailing" << endl;
				OutputError("Incomplete Input");
				return;
			}

			// Now lets call the parser:
			parsexml1(xmlReadMemory(buffer.c_str(), buffer.size(), "", "", 0));
					
						
		}
		// End of connect loop, we should never get here
		// UNLESS, some sort of quit request is implemented
			
	}
	// TODO: Put more specific catches, so I can OutputError with more
	// details as to what went wrong
	catch(eException &e)
	{
		cerr << "Exception caught: " << endl;
		cerr << strerror(errno) << endl;
		cerr << "Outputting XML error and bailing" << endl;
		OutputError("Exception Caught");
		return;
	}
}
		



void parsexml1(xmlDocPtr doc)
{
	xmlNodePtr root=NULL, curr=NULL;
	int request_type;
	configuration myconfig;
	myconfig.load();
	string name="";
	int id=0;
	filelist *myfilelist;



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
			if(strcmp((char *)curr->children->content, "Check") == 0)
			{
				request_type = CHECK;
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
			OutputInfo(); // No point in parsing any more, really...
			break;
		case CHECK:
			parsexml2(CHECK, curr);
			break;

		case CHECKALL:
			cerr << "Unimplemented Request (CheckAll)" << endl;
			OutputError("Unimplemented Request, CheckAll");
			break;

		case UP2DATE:
			cerr << "Unimplemented Request (Up2Date)" << endl;
			OutputError("Unimplemented Request, Up2Date");
			break;

		case UP2DATEALL:
			cerr << "Unimplemented Request (Up2DateAll)" << endl;
			OutputError("Unimplemented Request, Up2DateAll");
			break;
			
			

		default:
			cerr << "ERROR: Unhandled request_type" << endl;
			OutputError("Unhandled Request Type");
			return;
	}
				
}

void OutputError(string error)
{
	string output=""; // Static XML - can't be assed to get
	// libXML to output to a FD
	cerr << "Outputting error tree for error \"" << error << "\"" << endl;

	output="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<tuxcast>";
	output=output+"<version>" + PROTOCOL_VERSION + "</version>\n";
	output=output+"<server>Tuxcast Backend</server>\n"; // += seems to break these
	output=output+"<error>" + error + "</error>\n</tuxcast>\n";

	write(FD, output.c_str(), output.size());
	close(FD); // Usually the program quits straight after calling me - 
	// Might aswell close the connection here
}

void OutputInfo(void)
{
	string output="";
	
	cerr << "Outputting Info XML tree" << endl;

	output="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<tuxcast>\n";
	output=output+"<version>" + PROTOCOL_VERSION + "</version>\n";// += breaks these
	output=output+"<server>Tuxcast Backend</server>\n</tuxcast>\n";
		
	write(FD, output.c_str(), output.size());
	close(FD);
}

void parsexml2(int request_type, xmlNodePtr node)
{
	xmlNodePtr curr=0;
	switch(request_type)
	{
		case CHECK:
			// We just need to find the feed name, then call xmlcheck(feedname)
			curr = node;

			// Step into whitespace:
			curr = curr->next;
			// Loop till <feed>
			while(curr->type == XML_TEXT_NODE)
				curr = curr->next;
				

			xmlcheck((char *)curr->children->content);
	}
}


int idfromname(string name)
{
	configuration myconfig;
	myconfig.load();
	

	// Find the feed's ID
	for(int i=0; i<myconfig.feeds.size(); i++)
		if(strcmp(myconfig.feeds[i]->name.c_str(), name.c_str()) == 0)
			return i;

	
	// If we didn't return above:
	OutputError("Unknown feed");
	cerr << "Unknown feed, \"" << name << "\"" << endl;
	return -1;
}

void xmlcheck(string name)
{
	int id=0;
	configuration myconfig;
	filelist *myfilelist;
	string output="";
	
	myconfig.load();
	
	if((id=idfromname(name)) < 0)
		return;

	if(!(myfilelist = parse(myconfig.feeds[id]->address)))
	{
		OutputError("Cannot parse feed");
		return;
	}

	// New static output:
	output="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	output=output+"<tuxcast>\n";
	output=output+"<version>"+PROTOCOL_VERSION+"</version>\n";
	output=output+"<server>Tuxcast Backend</server>\n";
	output=output+"<output>\n<feed>\n";
	output=output+"<name>"+name+"</name>\n";
	
	
	for(int i=0; i<myfilelist->files.size(); i++)
	{
		// For each episode, add a new node to feed:

		// Static..:
		output=output+"<episode>\n";
		output=output+"<filename>"+myfilelist->files[i]->filename+"</filename>\n";
		output=output+"<url>"+myfilelist->files[i]->URL+"</url>\n";
		output=output+"<status>"+xmlget(myfilelist->files[i]->filename, myfilelist->files[i]->URL, myconfig.feeds[id]->folder)+"</status>\n";
		// Urgh, that last line calls xmlget with all the right parameters, then sticks the output in output
		output=output+"</episode>\n";
	}
			
	// Finish off the doc:
	output=output+"</feed>\n</output>\n</tuxcast>\n";

	write(FD, output.c_str(), output.size());
	close(FD);
}

// Also, should this function kill the whole request with OutputError if a request dies?
// (probably not, as half the tree is already outputted)
string xmlget(string filename, string url, string folder)
{
	FILE *outputfile=NULL;
	CURL *mycurl;
	string path;
	configuration myconfig;
	myconfig.load();
	mycurl = curl_easy_init();
	if(mycurl == NULL)
		return "Error";
	if(alreadydownloaded(filename))
	{
		cerr << "Skipping file, " << filename << endl;
		return "Skipped";
	}

	cerr << "Downloading \"" << url << "\" to filename \"" << filename << "\"" << endl;
	path = myconfig.podcastdir;
	path += "/";
	path += folder;
	if(!checkfolderexists(path))
		return "Error";
	path += "/";
	path += filename;

	outputfile = fopen(path.c_str(), "w");
	if(outputfile == NULL)
		return "Error";

	curl_easy_setopt(mycurl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(mycurl, CURLOPT_WRITEDATA, outputfile);
	curl_easy_setopt(mycurl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_perform(mycurl);
	fclose(outputfile);
	newfile(filename);

	curl_easy_cleanup(mycurl);
	
}


