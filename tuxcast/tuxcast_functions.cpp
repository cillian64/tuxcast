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



#include "../compile_flags.h"
#include <iostream>
#include "rss.h"
#include "../config/config.h"
#include <curl/curl.h>
#include <cstdio> // Needed to open a file in the classic way, so libcurl can write to it
#include <libxml/tree.h>   // V----------------V
#include <libxml/parser.h> // for filelist stuff
#include <unistd.h>
#include "tuxcast.h"
#include "../libraries/filestuff.h"
#include "../libraries/filestuff_exceptions.h"
#include "../config/config_exceptions.h"
#include "rss_exceptions.h"
#include "tuxcast_functions.h"

using namespace std;



		
// Check a particular feed
void check(configuration *myconfig, int feed)
{
	filelist *myfilelist;
	
	if(!(myfilelist = parsefeed(myconfig->feeds[feed]->address)))
		return;


	for(int j=0, size=myfilelist->files.size(); j<size; j++)
	{
		try
		{
			get(myfilelist->files[j]->filename, myfilelist->files[j]->URL,
				feed, myconfig);
		}
		catch(eFilestuff_CannotCreateFolder &e)
		{
			cerr << "Oops, couldn't create folder for feed \"" << myconfig->feeds[feed]->name << "\"" << endl;
			cerr << "Exception caught: ";
			e.print();
			return; // Skip this feed:
			// If we can't create the folder, no point in trying to
			// get any other files
		}
	}

}



// Downloads the latest episode on a particular feed
// Adds other episodes to files.xml without downloading
void up2date(configuration *myconfig, int feed)
{
	filelist *myfilelist;
	
	if(!(myfilelist = parsefeed(myconfig->feeds[feed]->address)))
		return;

		
	for(int j=0, size=myfilelist->files.size(); j<size; j++)
	{
		if(j==0)
			get(myfilelist->files[j]->filename, myfilelist->files[j]->URL,
					feed, myconfig);
		else
			newfile(myfilelist->files[j]->filename);
		// First file, download it
		// Other files, just pretend
	}

}




// This loops through all feeds and passes them to check()
void checkall(configuration *myconfig)
{
	for(int i=0; i<myconfig->feeds.size(); i++)
	{
		cout << "Checking feed \"" << myconfig->feeds[i]->name << "\"" << endl;
		check(myconfig, i);
	}
}

// This loops through all feeds and passes them to up2date()
void up2dateall(configuration *myconfig)
{
	// God knows why we had a filelist pointer here
	for(int i=0; i<myconfig->feeds.size(); i++)
	{
		cout << "up2date'ing feed \"" << myconfig->feeds[i]->name << "\"..." << endl;
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
   	if(strcasecmp((char*)curr->name,"file") == 0)
   	{
                 if(strcasecmp((char *)curr->children->content,name.c_str()) == 0)
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

	if(strcasecmp(temp.c_str(),"yes") == 0)
	{
		cout << "Downloading \"";
		cout << name;
		cout << "\"..." << endl;

		// Do folder'y stuff first
		path = myconfig->podcastdir;
		path += "/";
		path += myconfig->feeds[feed]->folder;
		
		checkfolderexists(path);
		// If anything goes wrong here, the exception should
		// abort everything...
		
		path += "/";
		path += name;
		
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

filelist *parsefeed(string URL) // This parses the feed, with error checking.
// It returns NULL if any error occured
{
	filelist *myfilelist;
	try
	{
		myfilelist = parse(URL);
	}
	catch(eRSS_InvalidRootNode &e)
	{
		cerr << "This is an XML file, but does not appear to be RSS 1 or RSS 2" << endl;
		cerr << "Please check you have the correct URL, and that it is an RSS feed, " \
		 << "and then contact your feed maintainer" << endl;
		 cerr << "Exception caught:";
		 e.print();
		 cerr << "Aborting this feed." << endl;
		 return NULL;
	}
	catch(eRSS_CannotParseFeed &e)
	{
		cerr << "Couldn't parse feed." << endl;
		cerr << "Please check the URL is correct, then contact your feed maintainer." << endl;
		cerr << "Exception caught: ";
		e.print();
		cerr << "Aborting this feed." << endl;
		return NULL;
	}
	return myfilelist;
}

