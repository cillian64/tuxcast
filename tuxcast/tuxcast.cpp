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




const char options[] = "cuC:U:b";

int main(int argc, char *argv[])
{
	configuration myconfig;
	
	try
	{
		myconfig.load();
	}
	catch(eConfig_NoConfigFile &e)
	{
		cerr << "Cannot load config file - please create one" << endl;
		return -1; // No need to print the exception
		// We know exactly what this one means
	}
	// If theres no config file, then
	// a) checkfileexists will throw an exception, which will be
	// caught here, and we abort.
	// b) checkfileexists will return false

	
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
			for(int i=0; i<myconfig.feeds.size(); i++)
			{
				if(strcasecmp(optarg,myconfig.feeds[i]->name.c_str()) == 0)
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
			for(int i=0; i<myconfig.feeds.size(); i++)
			{
				if(strcasecmp(optarg,myconfig.feeds[i]->name.c_str()) == 0)
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

		case 'b':
			// Backend mode:
			cerr << "Backend mode gone for now" << endl;
			cerr << "See the tuxcast blog, tuxcast.sf.net" << endl;
			break;


		default:
			cout << "Usage: tuxcast <option>" << endl;
			cout << "Where <option> is either -c or -u" << endl;
			cout << "-c - Check all feeds" << endl;
			cout << "-u - Download only the latest file from all feeds" << endl;
			cout << "-C NAME - check the specified feed" << endl;
			cout << "-U name - download only the latest episode from the specified feed" << endl;
			cout << "-b - enter the XML backend mode" << endl;
			cout << endl;
	}

	return 0;
}

