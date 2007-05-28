/*
 * 
 * This file is part of Tuxcast, "The linux podcatcher"
 * Copyright (C) 2006-2007 David Turner
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
#include <stdio.h>
#include <libxml/tree.h>   // V----------------V
#include <libxml/parser.h> // for filelist stuff
#include <unistd.h>
#include "tuxcast.h"
#include "../libraries/filestuff.h"
#include "../libraries/filestuff_exceptions.h"
#include "../config/config_exceptions.h"
#include "rss_exceptions.h"
#include "tuxcast_functions.h"

#include <libintl.h>
#include <locale.h>

#define _(x) gettext(x)

using namespace std;

// Check a particular feed
void check(configuration *myconfig, int feed)
{
	filelist *myfilelist;
	
	cachefeed(myconfig->feeds[feed]->name, myconfig->feeds[feed]->address);
	if(!(myfilelist = parsefeed(myconfig->feeds[feed]->name)))
		return;
	// TODO: Umm, I really should return an error or warning or something,
	// shouldn't I...?

	for(int j=0, size=myfilelist->size(); j<size; j++)
	{
		try
		{
			get((*myfilelist)[j]->filename, (*myfilelist)[j]->URL,
				feed, myconfig);
		}
		catch(eFilestuff_CannotCreateFolder &e)
		{
			fprintf(stderr,_("Oops, couldn't create folder for feed \"%s\"\n"),myconfig->feeds[feed]->name.c_str());
			fprintf(stderr,_("Exception caught: ")); // Yes, no \n.
			e.print();
			return; // Skip this feed:
			// If we can't create the folder, no point in trying to
			// get any other files from this feed
		}
	}

}



// Downloads the latest episode on a particular feed
// Adds other episodes to files.xml without downloading
void up2date(configuration *myconfig, int feed)
{
	filelist *myfilelist;
	
	cachefeed(myconfig->feeds[feed]->name,myconfig->feeds[feed]->address);

	if(!(myfilelist = parsefeed(myconfig->feeds[feed]->name)))
		return;

		
	for(int j=0, size=myfilelist->size(); j<size; j++)
	{
		if(j==0)
			get((*myfilelist)[j]->filename, (*myfilelist)[j]->URL,
					feed, myconfig);
		else
			newfile((*myfilelist)[j]->filename);
		// First file, download it
		// Other files, just pretend
	}

}




// This loops through all feeds and passes them to check()
void checkall(configuration *myconfig)
{
	for(int i=0; i<myconfig->feeds.size(); i++)
	{
		printf(_("Checking feed \"%s\"\n"),myconfig->feeds[i]->name.c_str());
		check(myconfig, i);
	}
}

// This loops through all feeds and passes them to up2date()
void up2dateall(configuration *myconfig)
{
	// God knows why we had a filelist pointer here
	for(int i=0; i<myconfig->feeds.size(); i++)
	{
		printf(_("up2date'ing feed \"%s\"\n"),myconfig->feeds[i]->name.c_str());
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
		fprintf(stderr,_("Error initializing libcurl\n"));
		return;
	}
	
	if(alreadydownloaded(name))
		// Already downloaded
		return;
	
	if(myconfig->ask == true)
	{
		printf(_("Download %s? (yes/no)\n"));
		cin >> temp; // TODO: Replace this with scanf?
	}
	else
		temp = "yes";

	if(strcasecmp(temp.c_str(),"yes") != 0)
		return;

	printf(_("Downloading \"%s\"..."),name.c_str());

	// Do folder'y stuff first
	path = myconfig->podcastdir;
	path += "/";
	// If the podcast's folder is absolute, don't prepend podcastdir
	if(myconfig->feeds[feed]->folder[0] == '/')
		path = myconfig->feeds[feed]->folder;
	else
		path += myconfig->feeds[feed]->folder;
	checkfolderexists(path);
	// If anything goes wrong here, the exception should
	// abort everything...
	
	path += "/";
	path += name;
	
	outputfile = fopen(path.c_str(), "w");
	if(outputfile == NULL)
	{
		fprintf(stderr,_("Error opening output file \"%s\"\n"),path.c_str());
		return; // Abort download
	}
	curl_easy_setopt(mycurl,CURLOPT_URL,URL.c_str());
	curl_easy_setopt(mycurl,CURLOPT_WRITEDATA,outputfile);
	curl_easy_setopt(mycurl,CURLOPT_FOLLOWLOCATION,1);
	curl_easy_perform(mycurl);
	fclose(outputfile);
	newfile(name);

	curl_easy_cleanup(mycurl);
}

void cachefeed(string name, string URL)
{ // Yel, like, I didn't just copy/paste get() and change a couple of bits :P
	CURL *mycurl;
	FILE *outputfile=NULL;
	string path;
	mycurl = curl_easy_init();
	if(mycurl == NULL)
	{
		fprintf(stderr, _("Error initializing libcurl\n"));
		return;
	}

	// Do folder'y stuff first
	path = getenv("HOME");
	path+= "/.tuxcast/cache";
	// If the postcast's folder is absolute, don't prepend a podcastdir
	checkfolderexists(path);
	// If anything goes wrong here, the exception should
	// abort everything...
	
	path += "/";
	path += name;

	outputfile = fopen(path.c_str(), "w");
	if(outputfile == NULL)
	{
		fprintf(stderr,_("Error opening output file \"%s\".\n"),
			path.c_str());
		return;
	}
	curl_easy_setopt(mycurl,CURLOPT_URL,URL.c_str());
	curl_easy_setopt(mycurl,CURLOPT_WRITEDATA,outputfile);
	curl_easy_setopt(mycurl,CURLOPT_FOLLOWLOCATION,1);
	curl_easy_perform(mycurl);
	fclose(outputfile);
	newfile(name);

	curl_easy_cleanup(mycurl);
}
	
filelist *parsefeed(string name) // This parses the feed, with error checking.
// It returns NULL if any error occured
{
	filelist *myfilelist;
	string URL=getenv("HOME");
	URL+="/.tuxcast/cache/";
	URL+=name;
	try
	{
		myfilelist = parse(URL);
	}
	catch(eRSS_InvalidRootNode &e)
	{
		 fprintf(stderr,_("This is an XML file, but does not appear to be RSS 1 (RDF) or RSS 2\n"));
		 fprintf(stderr,_("Please check you have the correct URL and that it is an RSS feed, and then contact your feed maintainer\n"));
		 fprintf(stderr,_("Exception caught:")); // No \n, I know
		 e.print();
		 fprintf(stderr,_("Aborting this feed.\n"));
		 return NULL;
	}
	catch(eRSS_CannotParseFeed &e)
	{
		fprintf(stderr,_("Couldn't parse the feed.\n"));
		fprintf(stderr,_("Please check the URL is correct, then contact your feed maintainer.\n"));
		fprintf(stderr,_("Exception caught: ")); // No \n
		e.print();
		fprintf(stderr,_("Aborting this feed.\n"));

		return NULL;
	}
	return myfilelist;
}

