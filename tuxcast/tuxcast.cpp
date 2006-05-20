#include "../compile_flags.h"
#include <iostream>
#include "rss.h"
#include "../config/config.h"
#include <curl/curl.h>
#include <cstdio> // Needed to open a file in the classic way, so libcurl can write to it
#include <libxml/tree.h>   // V----------------V
#include <libxml/parser.h> // for filelist stuff
#include <unistd.h>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

using namespace std;

#define fs boost::filesystem

void newfile(string name);
bool alreadydownloaded(string name); // true if already downloaded
void check(configuration *myconfig, int feed);
void up2date(configuration *myconfig, int feed);
void checkall(configuration *myconfig);
void up2dateall(configuration *myconfig);
void get(string name, string URL, int feed, configuration *myconfig);

const char options[] = "cuC:U:";

int main(int argc, char *argv[])
{
	configuration myconfig;
	
	// This must only be done ONCE!!!!!!!
	// Hence, it's done in the main program, instead of in any libraries
	// Maybe create an "init" function??

	try
	{
#ifdef POSIX
		fs::path::default_name_check(fs::portable_posix_name);
#endif
#ifdef WINDOWS
		fs::path::default_name_check(fs::windows_name);
#endif
	}
	catch(...)
	{
		// The only reason default_name_check should fail is if
		// It's called twice.  This could be caused by:
		cerr << "ERROR: you have both WINDOWS and POSIX compile flags enabled." << endl;
		cerr << "If you compiled manually, check your compile_flags.h" << endl;
		cerr << "Else, contact your package maintainer" << endl;
	}

	// We MUST setup the default_name_check before calling load():
	// load() uses some boost stuff which will mess up badly if
	// the default_name_check isn't POSIX or WINDOWS
	
	myconfig.load();

	
	switch(getopt(argc,argv,options))
	{
		case 'c':
			cout << "Checking all feeds" << endl;
			checkall(&myconfig);
			break;
		case 'u':
			cout << "Getting up to date on all feeds" << endl;
			up2dateall(&myconfig);
			break;

		case 'C':
			cout << "Checking feed, \"" << optarg << "\"" << endl;
			// We need to loop through myconfig.feeds to find the feed ID corresponding to the passed name
			if(strcmp(optarg,"") == 0)
			{
				cerr << "You must pass a non-blank feed name" << endl;
				return -1;
			}
			for(int i=0; i<myconfig.numoffeeds; i++)
			{
				if(strcmp(optarg,myconfig.feeds[i].name.c_str()) == 0)
				{
					// Found the feed
					check(&myconfig, i);
					return 0;
				}
			}
			// If we got all through the feeds, and it wasn't found (and we returned),
			// then the feed doesn't exist:
			cerr << "Unknown feed, \"" << optarg << "\"" << endl;
			return -1;
			break; // Bah
		case 'U':
			cout << "Getting up2date on feed, \"" << optarg << "\"" << endl;
			// We need to loop through myconfig.feeds to find the feed ID corresponding to the passed name
			if(strcmp(optarg,"") == 0)
			{
				cerr << "You must pass a non-blank feed name" << endl;
				return -1;
			}
			for(int i=0; i<myconfig.numoffeeds; i++)
			{
				if(strcmp(optarg,myconfig.feeds[i].name.c_str()) == 0)
				{
					// Found the feed
					up2date(&myconfig, i);
					return 0;
				}
			}
			// If we got all through the feeds, and it wasn't found (and we returned),
			// then the feed doesn't exist:
			cerr << "Unknown feed, \"" << optarg << "\"" << endl;
			return -1;
			break; // Bah


		default:
			cout << "Usage: tuxcast <option>" << endl;
			cout << "Where <option> is either -c or -u" << endl;
			cout << "-c - Check all feeds" << endl;
			cout << "-u - Download only the latest file from all feeds" << endl;
			cout << "-C NAME - check the specified feed" << endl;
			cout << "-U name - download only the latest episode from the specified feed" << endl;
			cout << endl;
	}

	return 0;
}
			
// Check a particular feed
void check(configuration *myconfig, int feed)
{
	filelist *myfilelist;
	
	myfilelist = parse(myconfig->feeds[feed].address);
	if(myfilelist == NULL)
	{
		cerr << "*** parse() failed - aborting this feed ***" << endl;
		cerr << "*** Check the URL is right, then go moan to your feed maintainer :-) ***" << endl;
		return;
	}

	for(int j=0, size=myfilelist->numoffiles(); j<size; j++)
	{
		get(myfilelist->getfilename(j), myfilelist->getURL(j),
				feed, myconfig);
	}

}



// Downloads the latest episode on a particular feed
// Adds other episodes to files.xml without downloading
void up2date(configuration *myconfig, int feed)
{
	filelist *myfilelist;
	
	myfilelist = parse(myconfig->feeds[feed].address);
	if(myfilelist == NULL)
	{
		cerr << "*** parse() failed - aborting this feed ***" << endl;
		cerr << "*** Check the URL is right, then go moan to your feed maintainer :-) ***" << endl;
		return;
	}

		
	for(int j=0, size=myfilelist->numoffiles(); j<size; j++)
	{
		if(j==0)
			get(myfilelist->getfilename(j), myfilelist->getURL(j),
					feed, myconfig);
		else
			newfile(myfilelist->getfilename(j));
		// First file, download it
		// Other files, just pretend
	}

}




// This loops through all feeds and passes them to check()
void checkall(configuration *myconfig)
{
	for(int i=0; i<myconfig->numoffeeds; i++)
	{
		cout << "Checking feed \"" << myconfig->feeds[i].name << endl;
		check(myconfig, i);
	}
}

// This loops through all feeds and passes them to up2date()
void up2dateall(configuration *myconfig)
{
	filelist *myfilelist;
	
	for(int i=0; i<myconfig->numoffeeds; i++)
	{
		cout << "up2date'ing feed \"" << myconfig->feeds[i].name << "\"..." << endl;
		up2date(myconfig,i);
	}
	
}
                                                          
// This adds a file to files.xml
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


// This returns true if a file is already in files.xml
bool alreadydownloaded(string name)
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

// Download an episode
void get(string name, string URL, int feed,  configuration *myconfig)
{
	string temp;
	FILE *outputfile=NULL;
	CURL *mycurl;
	string path;
	mycurl = curl_easy_init();
	if(mycurl == NULL)
	{
		cerr << "Error initializing libcurl" << endl;
		return;
	}
	
	if(alreadydownloaded(name))
		// Already downloaded
		return;
	
	if(myconfig->ask == true)
	{
		cout << "Download ";
		cout << name;
		cout << "?: (yes/no)" << endl;
		cin >> temp;
	}
	else
		temp = "yes";

	if(strcmp(temp.c_str(),"yes") == 0)
	{
		cout << "Downloading \"";
		cout << name;
		cout << "\"..." << endl;

		// Do folder'y stuff first
		path = myconfig->podcastdir;
		path += "/";
		path += myconfig->feeds[feed].folder;
		if(!fs::exists(path))
		{
			try
			{
				fs::create_directory(path);
			}
			catch(...)
			{
				cerr << "Error creating folder, \"" << path << "\"" << endl;
				cerr << "Aborting..." << endl;
				return;
			}
		}
		

		// Done with the folder now
		// Onto files
		path += "/";
		path += name;
		// Path = previousfolder/filename.XYZ
		
		outputfile = fopen(path.c_str(), "w");
		if(outputfile == NULL)
		{
			cerr << "Error opening output file \"";
			cerr << path << "\"" << endl;

		}
		curl_easy_setopt(mycurl,CURLOPT_URL,URL.c_str());
		curl_easy_setopt(mycurl,CURLOPT_WRITEDATA,outputfile);
		curl_easy_setopt(mycurl,CURLOPT_FOLLOWLOCATION,1);
		curl_easy_perform(mycurl);
		fclose(outputfile);
		newfile(name);
	}

	curl_easy_cleanup(mycurl);
}

