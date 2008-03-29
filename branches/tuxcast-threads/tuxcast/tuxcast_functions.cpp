/*
 * 
 * This file is part of Tuxcast, "The linux podcatcher"
 * Copyright (C) 2006-2008 David Turner
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
#ifdef PCREPP
#include <pcre++.h>
#endif
#ifdef TORRENT
#include "../libraries/torrent/torrent.h"
#endif
#ifdef THREADS
#include <pthread.h>
#endif

#include <libintl.h>
#include <locale.h>

#define _(x) gettext(x)

#include "tuxcast_exceptions.h"
#include "tuxcast.h"
#include "../libraries/filestuff.h"
#include "../libraries/filestuff_exceptions.h"
#include "../libraries/common.h"
#include "../config/config_exceptions.h"
#include "rss_exceptions.h"
#include "tuxcast_functions.h"


using namespace std;

// Check a particular feed
void check(configuration &myconfig, feed &feed, filelist &allfiles)
{
	cachefeed(feed.name, feed.address);

	auto_ptr<filelist> myfilelist = parsefeed(feed.name);
	if(!myfilelist.get())
		return;
	// TODO: Umm, I really should return an error or warning or something,
	// shouldn't I...?

	FOREACH(filelist::iterator, *myfilelist, file)
	{	
		if(alreadydownloaded(file->filename))
			continue;
		file->parentfeed = &feed;
#ifdef THREADS
		if(myconfig.numofthreads < myconfig.numofdownloaders)
		{
				pthread_mutex_lock(&(myconfig.configlock));
			myconfig.numofthreads++;
			pthread_mutex_unlock(&(myconfig.configlock));
			struct threaddata *data = new struct threaddata;
			data->thefile = *file;
			data->myconfig = &myconfig;
			data->allfiles = &allfiles;
			myconfig.threads.push_back(0);
			pthread_create(&(myconfig.threads[myconfig.threads.size()-1]),
				NULL, threadfunc, (void*)data);
		}
		else
			allfiles.push_back(*file);
#else
		allfiles.push_back(*file);
#endif

	}
}

#ifdef THREADS
void *threadfunc(void *data)
{
	struct threaddata *tdata = (struct threaddata *)data;
	file thefile;

	/* Get the file originally spawned for */
	get(tdata->thefile, *(tdata->myconfig));

	/* Start nabbing the rest of the files */
	while(true)
	{
		pthread_mutex_lock(&(tdata->myconfig->configlock));
		if(tdata->allfiles->size() == 0)
		{
			pthread_mutex_unlock(&(tdata->myconfig->configlock));
			break;
		}
		thefile = tdata->allfiles->front();
		tdata->allfiles->erase(tdata->allfiles->begin());
		pthread_mutex_unlock(&(tdata->myconfig->configlock));
		get(thefile, *(tdata->myconfig));
	}
	

	pthread_mutex_lock(&(tdata->myconfig->configlock));
	tdata->myconfig->numofthreads--;
	pthread_mutex_unlock(&(tdata->myconfig->configlock));

	delete (struct threaddata*)data;

}
#endif

// Downloads the latest episode on a particular feed
// Adds other episodes to files.xml without downloading
void up2date(configuration &myconfig, feed &feed, filelist &allfiles)
{
	cachefeed(feed.name, feed.address);

	auto_ptr<filelist> myfilelist = parsefeed(feed.name);
	if(!myfilelist.get())
		return;

	FOREACH(filelist::iterator, *myfilelist, file)
	{
		if(alreadydownloaded(file->filename))
			continue;

		file->parentfeed = &feed;

		if(file == myfilelist->begin())
#ifdef THREADS
			if(myconfig.numofthreads < myconfig.numofdownloaders)
			{
				struct threaddata *data = new struct threaddata;
				data->thefile = *file;
				data->myconfig = &myconfig;
				data->allfiles = &allfiles;
				myconfig.threads.push_back(0);
				pthread_create(&(myconfig.threads[myconfig.threads.size()-1]),
					NULL, threadfunc, (void*)data);
			}
			else
				allfiles.push_back(*file);
#else
			allfiles.push_back(*file);
#endif
		else
			newfile(file->filename);
	}
}




// This loops through all feeds and passes them to check()
void checkall(configuration &myconfig)
{
	filelist allfiles;
	FOREACH(configuration::feedlist::iterator, myconfig.feeds, feed)
	{
		printf(_("Checking feed \"%s\"\n"),feed->name.c_str());
		
		check(myconfig, *feed, allfiles);
	}
	getlist(allfiles, myconfig);
	/* Threads must be joined before allfiles is trashed */
#ifdef THREADS
	for(int i=0; i<myconfig.threads.size(); i++)
		pthread_join(myconfig.threads[i], NULL);
#endif
}

// This loops through all feeds and passes them to up2date()
void up2dateall(configuration &myconfig)
{
	filelist allfiles;
	// God knows why we had a filelist pointer here
	FOREACH(configuration::feedlist::iterator, myconfig.feeds, feed)
	{
		printf(_("up2date'ing feed \"%s\"\n"),feed->name.c_str());
		up2date(myconfig,*feed, allfiles);
	}
	getlist(allfiles, myconfig);
#ifdef THREADS
	for(int i=0; i<myconfig.threads.size(); i++)
		pthread_join(myconfig.threads[i], NULL);
#endif

}
                                                          
// This adds a file to files.xml
void newfile(string name)
{
        // Load the current filelist:
        xmlDoc *doc;
        xmlNode *root, curr;
        string path=getenv("HOME");
        path += "/.tuxcast-thread/files.xml";
	char *temppath=new char[path.size()+7];
	strcpy(temppath,path.c_str());
	strcat(temppath,".XXXXXX");
	mkstemp(temppath);
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
        if(xmlSaveFormatFileEnc(temppath, doc, "UTF-8", 1) == -1)
		throw eTuxcast_FSFull();
	if(!move(temppath,path))
		throw eTuxcast_FSFull();
}


// This returns true if a file is already in files.xml
bool alreadydownloaded(string name)
{
        // Load the filelist:
        xmlDoc *doc;
        xmlNode *root,*curr;
        string path = getenv("HOME");
        path += "/.tuxcast-thread/files.xml";
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
		 if(curr->children != NULL) /* Empty <file> tags are possible... */
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
void get(file &thefile, configuration &myconfig)
{
	string temp;
	FILE *outputfile=NULL;
	CURL *mycurl;
	bool correctmime=false;
	mycurl = curl_easy_init();
	if(mycurl == NULL)
	{
		fprintf(stderr,_("Error initializing libcurl\n"));
		return;
	}
	
	thefile.filename.c_str();
//	if(alreadydownloaded(thefile.filename))
//		// Already downloaded
//		return;
	
	if(myconfig.permitted_mimes.size() == 0)
		correctmime=true; // If they haven't specified any permitted
		// MIMEs, we'll assume they want all files rather than no
		// files... Don't output an error either.
	
	if(strcmp(thefile.type.c_str(),"") == 0)
		correctmime=true; // If the MIME type of the file in the RSS
		// feed is blank, we'll download it anyway...
	
	FOREACH(configuration::mimelist::iterator, myconfig.permitted_mimes, mime)
		if(strcasecmp(mime->c_str(), thefile.type.c_str()) == 0)
			correctmime=true;

	if(correctmime == false) // The MIME type isn't blank, the permitted
	// list isn't empty, and it ain't permitted...
	{
		fprintf(stderr,_("Not downloading %s - incorrect MIME type\n"),
			thefile.filename.c_str());

		newfile(thefile.filename);
		return;
	}
	
	if(myconfig.ask == true)
	{
		printf(_("Download %s? (yes/no)\n"), thefile.filename.c_str());
		cin >> temp; // TODO: Replace this with scanf?
	}
	else
		temp = "yes";

	if(strcasecmp(temp.c_str(),"yes") != 0)
	{
		newfile(thefile.filename);
		return;
	}

	populate_download_path(*(thefile.parentfeed), thefile, myconfig);

	outputfile = fopen(thefile.savepath.c_str(), "w");

	if(outputfile == NULL)
	{
		fprintf(stderr,_("Error opening output file \"%s\"\n"),thefile.savepath.c_str());
		// TODO: throw exception
		return; // Abort download
	}
	

	printf(_("Downloading %s...\n"),thefile.filename.c_str());
	
	curl_easy_setopt(mycurl,CURLOPT_URL,thefile.URL.c_str());
	curl_easy_setopt(mycurl,CURLOPT_WRITEDATA,outputfile);
	curl_easy_setopt(mycurl,CURLOPT_FOLLOWLOCATION,1);
	curl_easy_setopt(mycurl,CURLOPT_NOPROGRESS,1);
	curl_easy_perform(mycurl);
	fclose(outputfile);
	newfile(thefile.filename);
	
	curl_easy_cleanup(mycurl);
}

void populate_download_path(feed &feed, file &file, configuration &myconfig)
{
	// Work out path to download file
	string path;
	// If the podcast's folder is absolute, don't prepend podcastdir
	if(feed.folder[0] == '/')
	{
		path = feed.folder;
	}
	else
	{
		path = myconfig.podcastdir;
		path += "/";
		path += feed.folder;
	}
	checkfolderexists(path);
	// If anything goes wrong here, the exception should
	// abort everything...
	
	path += "/";
	path += file.filename;

	file.savepath = path;
}


void getlist(filelist &files, configuration &myconfig)
{
	FOREACH(filelist::iterator, files, file)
	{
		try
		{
			get(*file, myconfig);
		}
		catch(eFilestuff_CannotCreateFolder &e)
		{
			fprintf(stderr, _("Oops, couldn't create folder for feed\n"));
			fprintf(stderr, _("Exception caught: "));
			e.print();
		}
	}
	// Download all files via HTTP before bittorrent
	FOREACH(filelist::iterator, files, file)
	{
		if(file->filename.size() <= 8) // Name this short cannot
			continue; // Be a .torrent file.
		char check[9];
		strncpy(check, file->filename.c_str() + file->filename.size() - 8, 9);
		if(strcasecmp(check, ".torrent") == 0)
			handle_bittorrent(*file);
	}

}

void handle_bittorrent(file &file)
{
	printf("Downloading %s via bittorrent\n", file.filename.c_str());
	bittorrent(file.savepath);
	printf("Finished %s\n",file.filename.c_str());
}

void cachefeed(const string &name, const string &URL)
{ // Yeh, like, I didn't just copy/paste get() and change a couple of bits :P
	CURL *mycurl;
	FILE *outputfile=NULL;
	string path;
	mycurl = curl_easy_init();
	if(mycurl == NULL)
	{
		cerr << "Error initializing libcurl" << endl;
		return;
	}
	
	// Do folder'y stuff first
	path = getenv("HOME");
	path += "/.tuxcast";
	checkfolderexists(path);
	path += "/cache";
	// If the podcast's folder is absolute, don't prepend podcastdir
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
		return;
	}
	curl_easy_setopt(mycurl,CURLOPT_URL,URL.c_str());
	curl_easy_setopt(mycurl,CURLOPT_WRITEDATA,outputfile);
	curl_easy_setopt(mycurl,CURLOPT_FOLLOWLOCATION,1);
	curl_easy_perform(mycurl);
	fclose(outputfile);
	
	curl_easy_cleanup(mycurl);
}

auto_ptr<filelist> parsefeed(string name) // This parses the feed, with error checking.
// It returns NULL if any error occured
{
	auto_ptr<filelist> myfilelist;
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
	}
	catch(eRSS_CannotParseFeed &e)
	{
		fprintf(stderr,_("Couldn't parse the feed.\n"));
		fprintf(stderr,_("Please check the URL is correct, then contact your feed maintainer.\n"));
		fprintf(stderr,_("Exception caught: ")); // No \n
		e.print();
		fprintf(stderr,_("Aborting this feed.\n"));
	}
	// myfilelist will be non-NULL if and only if parse(URL) ran okay
	return myfilelist;
}

