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
#include <stdio.h> // :)
#include "rss.h"
#include "../config/config.h"
#include <curl/curl.h>
#include <cstdio> // Needed to open a file in the classic way, so libcurl can write to it
#include <libxml/tree.h>   // V----------------V
#include <libxml/parser.h> // for filelist stuff
#include <unistd.h>
#include "tuxcast_functions.h"
#include "tuxcast_exceptions.h"
#include "../libraries/filestuff.h"
#include "../libraries/filestuff_exceptions.h"
#include "../libraries/common.h"
#include "../config/config_exceptions.h"
#include "rss_exceptions.h"
#include "tuxcast_functions.h"
#include "cleaner.h"
#include <map>
#include "../version.h"

#ifdef THREADS
#include <pthread.h>
#endif

using namespace std;


#include <libintl.h>
#include <locale.h>


const char options[] = "cuC:U:fvh";
#define _(x) gettext(x)


int main(int argc, char *argv[])
{
	configuration myconfig;
	map<char, string> vars;

        // TODO: Fix this stuff?:
	setlocale(LC_ALL,"");
	bindtextdomain("tuxcast","/usr/share/locale");
	textdomain("tuxcast");
	
	
	// Remember, we can only take one option.
	// We store the first option, then check - if there's another, we bork out and moan.
	
	char opt1=getopt(argc,argv,options);
	string optarg1;
	if(optarg != NULL)
		optarg1=optarg;
	else
		optarg1="";

	if(getopt(argc,argv,options) != -1)
	{
		cerr << "Error, more than one option was passed.  You must only pass one option." << endl;
		return -1;
	}

	if(!((opt1 == 'h') || (opt1 == 'v'))) // Load config if option isn't h or v
	{
		try
		{
			myconfig.load();
		}
		catch(eConfig_NoConfigFile &e)
		{
			fprintf(stderr,_("Cannot load config file - please create one\n"));
			return -1; // No need to print the exception
			// We know exactly what this one means
		}
		// If theres no config file, then
		// a) checkfileexists will throw an exception, which will be
		// caught here, and we abort.
		// b) checkfileexists will return false
	}

	try
	{
		switch(opt1)
		{
			case 'c':
				printf(_("Checking all feeds\n"));
				checkall(myconfig);
				setvars(vars, myconfig);
				runhook(POSTRUN, vars, myconfig);
				break;
			case 'u':
				printf(_("Getting up to date on all feeds\n"));
				up2dateall(myconfig);
				setvars(vars, myconfig);
				runhook(POSTRUN, vars, myconfig);
				break;

			case 'C':
				printf(_("Checking feed, \"%s\"\n"), optarg1.c_str());
				// We need to loop through myconfig.feeds to find the feed ID corresponding to the passed name
				if(strcmp(optarg1.c_str(),"") == 0)
				{
					fprintf(stderr,_("You must pass a non-blank feed name\n"));
					return -1;
				}
				FOREACH(configuration::feedlist::iterator, myconfig.feeds, feed)
				{
					if(strcasecmp(optarg1.c_str(),feed->name.c_str()) == 0)
					{
						// Found the feed
						filelist files;
						check(myconfig, *feed, files);
						getlist(files, myconfig);
#ifdef THREADS
						for(int i=0; i<myconfig.threads.size(); i++)
							pthread_join(myconfig.threads[i], NULL);
#endif
						setvars(vars, myconfig);
						runhook(POSTRUN, vars, myconfig);
						return 0;
					}
				}
				// If we got all through the feeds, and it wasn't found (and we returned),
				// then the feed doesn't exist:
				fprintf(stderr,_("Unknown feed, \"%s\"\n"),optarg1.c_str());
				return -1;
				break; // Bah
			case 'U':
				printf(_("Getting up2date on feed \"%s\"\n"),optarg1.c_str());
				// We need to loop through myconfig.feeds to find the feed ID corresponding to the passed name
				if(strcmp(optarg1.c_str(),"") == 0)
				{
					fprintf(stderr,_("You must pass a non-blank feed name\n"));
					return -1;
				}
				FOREACH(configuration::feedlist::iterator, myconfig.feeds, feed)
				{
					if(strcasecmp(optarg1.c_str(),feed->name.c_str()) == 0)
					{
						// Found the feed
						filelist files;
						up2date(myconfig, *feed, files);
						getlist(files, myconfig);
#ifdef THREADS
						for(int i=0; i<myconfig.threads.size(); i++)
							pthread_join(myconfig.threads[i], NULL);
#endif
						setvars(vars, myconfig);
						runhook(POSTRUN, vars, myconfig);
						return 0;
					}
				}
				// If we got all through the feeds, and it wasn't found (and we returned),
				// then the feed doesn't exist:
				fprintf(stderr,_("Unknown feed, \"%s\"\n"),optarg1.c_str());
				return -1;
				break; // Bah

			case 'f':
				cout << "Cleaning out files.xml..." << endl;
				clean();
				break;

			case 'v':
				version();
				break;

			case 'h':
				// Fallthrough:

			default:
				printf(_("Usage: tuxcast <option>\n"));
				printf(_("where <option> is one of the below:\n"));
				printf(_("-c - Check all feeds\n"));
				printf(_("-u - Download only the latest file from all feeds\n"));
				printf(_("-C name - Download all episodes of the named feed\n"));
				printf(_("-U name - Download only the latest episode of the named feed\n"));
				printf(_("-h - Show this help message\n"));
				printf(_("-f - clean out old podcasts from files.xml\n"));
				printf(_("-v - Show version and license information\n"));
		}
	}
	catch(eTuxcast_FSFull &e)
	{
		fprintf(stderr, _("Error: could not save config/state due to full filesystem:\n"));
		e.print();

		return -1;
	}

	return 0;
}

