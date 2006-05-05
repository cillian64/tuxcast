#include "../compile_flags.h"
#include <iostream>
#include "rss.h"
#include "../config/config.h"
#include <curl/curl.h>
#include <cstdio> // Needed to open a file in the classic way, so libcurl can write to it
#include <libxml/tree.h>   // V----------------V
#include <libxml/parser.h> // for filelist stuff
#include <unistd.h>
#include <boost/filesystem/path.hpp> // FOr the default_name_check init

using namespace std;

void newfile(string name);
bool checkfile(string name); // true if already downloaded
void checkall(void);
void up2date(void);

const char options[] = "cu";

int main(int argc, char *argv[])
{
	// This must only be done ONCE!!!!!!!
	// Hence, it's done in the main program, instead of in any libraries
	// Maybe create an "init" function??

	try
	{
#ifdef POSIX
		boost::filesystem::path::default_name_check(boost::filesystem::portable_posix_name);
#endif
#ifdef WINDOWS
		boost::filesystem::path::default_name_check(boost::filesystem::windows_name);
#endif
	}
	catch(...)
	{
		// The only reason default_name_check should fail is if
		cerr << "ERROR: you have both WINDOWS and POSIX compile flags enabled." << endl;
		cerr << "If you compiled manually, check your compile_flags.h" << endl;
		cerr << "Else, contact your package maintainer" << endl;
	}
	
	switch(getopt(argc,argv,options))
	{
		case 'c':
			cout << "Checking all feeds" << endl;
			checkall();
			break;
		case 'u':
			cout << "Getting up to date on all feeds" << endl;
			up2date();
			break;

		default:
			cout << "Usage: tuxcast <option>" << endl;
			cout << "Where <option> is either -c or -u" << endl;
			cout << "-c - Check all feeds" << endl;
			cout << "-u - Get up to date on all feeds - ";
			cout << "add files to files.xml but don't download";
			cout << endl;
	}

	return 0;
}
			


void up2date(void)
{
	filelist *myfilelist;
	string temp;
	FILE *outputfile;
	configuration myconfig;

	myconfig.load();
	for(int i=0; i<myconfig.numoffeeds; i++)
	{
		cout << "up2date'ing feed \"" << myconfig.feeds[i].name << "\"..." << endl;
		myfilelist = parse(myconfig.feeds[i].address);
		if(myfilelist == NULL)
		{
			cerr << "*** parse() failed - aborting this feed ***" << endl;
			cerr << "*** Check the URL is right, then go moan to your feed maintainer :-) ***" << endl;
			continue;
		}

			
		for(int j=0, size=myfilelist->numoffiles(); j<size; j++)
		{
			newfile(myfilelist->getfilename(j));
			
		}
	}
	
}

void checkall(void)
{
	filelist *myfilelist;
	string temp;
	FILE *outputfile=NULL;
	CURL *mycurl;
	configuration myconfig;
	string path;
	mycurl = curl_easy_init();
	if(mycurl == NULL)
	{
		cerr << "Error initializing libcurl" << endl;
		return;
	}
	
	myconfig.load();
	for(int i=0; i<myconfig.numoffeeds; i++)
	{
		cout << "Checking feed \"" << myconfig.feeds[i].name << "\"..." << endl;
		myfilelist = parse(myconfig.feeds[i].address);
		if(myfilelist == NULL)
		{
			cerr << "*** parse() failed - aborting this feed ***" << endl;
			cerr << "*** Check the URL is right, then go moan to your feed maintainer :-) ***" << endl;
			continue;
		}
		for(int j=0, size=myfilelist->numoffiles(); j<size; j++)
		{
			if(checkfile(myfilelist->getfilename(j)))
				continue;
			if(myconfig.ask == true)
			{
				cout << "Download ";
				cout << myfilelist->getfilename(j);
				cout << "?: (yes/no)" << endl;
				cin >> temp;
			}
			else
				temp = "yes";

			if(strcmp(temp.c_str(),"yes") == 0)
			{
				cout << "Downloading ";
				cout << myfilelist->getfilename(j);
				cout << "..." << endl;
		
				path = myconfig.podcastdir;
				path += "/";
				path += myfilelist->getfilename(j);
				outputfile = fopen(path.c_str(), "w");
				if(outputfile == NULL)
				{
					cerr << "Error opening output file" << endl;

				}
				curl_easy_setopt(mycurl,CURLOPT_URL,myfilelist->getURL(j).c_str());
				curl_easy_setopt(mycurl,CURLOPT_WRITEDATA,outputfile);
				curl_easy_perform(mycurl);
				fclose(outputfile);
				newfile(myfilelist->getfilename(j));
			}
		}
	}
	
	curl_easy_cleanup(mycurl);
	
}
                                                                               
void newfile(string name)
{
        // Load the current filelist:
        xmlDoc *doc;
        xmlNode *root, curr;
        string path=getenv("HOME");
        path += "/.tuxcast/files.xml";
        doc = xmlReadFile(path.c_str(), NULL, 0);
        if(doc == NULL)
        {
		doc = xmlNewDoc((xmlChar *)"1.0");
		root = xmlNewNode(NULL,(xmlChar *)"filelist");
		xmlDocSetRootElement(doc,root);
        }
	else
        	root = xmlDocGetRootElement(doc);


        // Add the new file:
        xmlNewChild(root,NULL,(xmlChar *)"file", (xmlChar *)name.c_str());
        // Save the filelist:
        xmlSaveFormatFileEnc(path.c_str(), doc, "UTF-8", 1);
}

bool checkfile(string name)
{
        // Load the filelist:
        xmlDoc *doc;
        xmlNode *root,*curr;
        string path = getenv("HOME");
        path += "/.tuxcast/files.xml";
        doc = xmlReadFile(path.c_str(),NULL,0);
        if(doc == NULL)
        {
                // No filelist, so we can be sure the file's not downloaded yet
                // A new filelist will be created when newfile() is called for this file
                return false; // Not downloaded
                // Alternately, there could be a syntax error, but since this file should be created/maintained by a (hopefully) sane program...
        }
        root = xmlDocGetRootElement(doc);
        curr = root->children;
        while(true) // is this portable...?
        {
   	if(strcmp((char*)curr->name,"file") == 0)
   	{
                 if(strcmp((char *)curr->children->content,name.c_str()) == 0)
                 {
                 	return true; // Already downloaded
                        break;
                 }
   	}
        if(curr->next == NULL)
        {
        	// No more nodes, if not already break'ed (or broken...?) then the file hasn't been downloaded
                	return false;
        }
        else
        	curr = curr->next;
        }
}
