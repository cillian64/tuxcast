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
#include <stdio.h> // :)
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
#include "../version.h"

using namespace std;


#include <libintl.h>
#include <locale.h>

const char options[] = "cuC:U:bvh";

#define _(x) gettext(x)

int main(int argc, char *argv[])
{
	configuration myconfig;

        // TODO: Fix this stuff?:
	setlocale(LC_ALL,"");
	bindtextdomain("tuxcast","/usr/share/locale");
	textdomain("tuxcast");
	
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

	
	switch(getopt(argc,argv,options))
	{
		case 'c':
			printf(_("Checking all feeds\n"));
			checkall(&myconfig);
			break;
		case 'u':
			printf(_("Getting up to date on all feeds\n"));
			up2dateall(&myconfig);
			break;

		case 'C':
			printf(_("Checking feed, \"%s\"\n"));
			// We need to loop through myconfig.feeds to find the feed ID corresponding to the passed name
			if(strcmp(optarg,"") == 0)
			{
				fprintf(stderr,_("You must pass a non-blank feed name\n"),optarg);
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
			fprintf(stderr,_("Unknown feed, \"%s\"\n"),optarg);
			return -1;
			break; // Bah
		case 'U':
			printf(_("Getting up2date on feed \"%s\"\n"),optarg);
			// We need to loop through myconfig.feeds to find the feed ID corresponding to the passed name
			if(strcmp(optarg,"") == 0)
			{
				fprintf(stderr,_("You must pass a non-blank feed name\n"));
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
			fprintf(stderr,_("Unknown feed, \"%s\"\n"),optarg);
			return -1;
			break; // Bah

		case 'b':
			// Backend mode:
			fprintf(stderr,_("Backend mode gone\nSee the tuxcast blog, http://tuxcast.sf.net/\n"));
			break;

		case 'v':
			version();
			break;

		case 'h':
			// Fallthrough:

		default:
			printf(_("Usage: tuxcast <option\n"));
			printf(_("where <option> is one of the below:\n"));
			printf(_("-c - Check all feeds\n"));
			printf(_("-u - Download only the latest file from all feeds\n"));
			printf(_("-C name - Download all episodes of the named feed\n"));
			printf(_("-U name - Download only the latest episode of the named feed\n"));
			printf(_("-h - Show this help message\n"));
			printf(_("-v - Show version and license information\n"));
	}

	return 0;
}

