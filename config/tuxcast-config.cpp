/*
 * 
 * This file is part of Tuxcast, "The linux podcatcher"
 * Copyright (C) 2006-2008 David Turner
 * Copyright (C) 2009 Mathew Cucuzella (kookjr@gmail.com)
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
#include "config.h"
#include <unistd.h>
#include "config_exceptions.h"
#include "../libraries/filestuff_exceptions.h"
#include "../libraries/common.h"
#include "../version.h"
#include <libintl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>

const char options[] = "aA:d:n:N:hs:g:Gf:umvt:r:p:";
#define _(x) gettext(x)

#ifndef MAX
#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

configuration myconfig;

void help(void);
void get(int argc, char **argv);
void getall(void);
void set();
void add(int argc,char *argv[]);
void del();
void update(int argc, char *argv[]);
void listmimes(void);
void addmime();
void removemime();

int main(int argc, char *argv[])
{
	// TODO: Insert internationalisation setup shizzle here:
	setlocale(LC_ALL,"");
	bindtextdomain("tuxcast","/usr/share/locale");
	textdomain("tuxcast");
	// TODO: Shouldn't that be tuxcast-config for the text domain?

	try
	{
		myconfig.load();
	}
	catch(eConfig_NoConfigFile &e)
	{
		fprintf(stderr,_("Warning, no config file\n"));
		fprintf(stderr,_("If you're changing an option or adding a feed, a config file will be created later\n"));
	}
	
	try
	{
		switch(getopt(argc,argv,options))
		{
			case 'a':
				add(argc,argv);
				break;
			
			case 'd':
				del();
				break;
			
			case 'g':
				get(argc,argv); // Get needs to use the options
				// for a few things.
				break;

			case 'G':
				getall();
				break;

			case 's':
				set();
				break;
			case 'u':
				update(argc,argv);
				break;

			case 'm':
				listmimes();
				break;

			case 't':
				addmime(); // Nothing is passed as the function
				// can access optarg...
				break;

			case 'r':
				removemime(); // "
				break;

			case 'v':
				version();
				break;

			case 'h':
				// Fallthrough
		
			default:
				help();
			}
	}
	catch(eConfig_CannotSaveConfig &e)
	{
		fprintf(stderr,_("Cannot save config file\n"));
		fprintf(stderr,_("Exception caught: "));
		e.print();
		fprintf(stderr,"\n");
		return -1;
	}
	// No point in catching that exception separatly in every action
	// Just catch here to avoid SIGABRT and exit cleanly
		
	return 0;
}

void listmimes(void)
{
	printf(_("Permitted MIME types:\n"));
	FOREACH(configuration::mimelist::iterator, myconfig.permitted_mimes, mime)
	{
		printf("%s\n",mime->c_str()); // Don't
		// add %s to the po file...
	}
}

void addmime(void)
{
	if(optarg == NULL)
	{
		fprintf(stderr,_("Optarg is NULL... this shouldn't happen.\n"));
		return; // TODO: exception?
	}
	FOREACH(configuration::mimelist::iterator, myconfig.permitted_mimes, mime)
	{
		if(strcasecmp(mime->c_str(),optarg) == 0)
		{
			fprintf(stderr,_("\"%s\" is already permitted\n"),optarg);
			return;
		}
	}

	myconfig.permitted_mimes.push_back(optarg);
	printf(_("\"%s\" added to list of permitted MIME types\n"),optarg);
	
	myconfig.save();
}

void removemime(void)
{
	if(optarg == NULL)
	{
		fprintf(stderr,_("Optarg is NULL... this shouldn't happen.\n"));
		return; // "
	}
	
	FOREACH(configuration::mimelist::iterator, myconfig.permitted_mimes, mime)
	{
		if(strcasecmp(mime->c_str(),optarg) == 0)
		{
			printf(_("\"%s\" is no longer permitted\n"),optarg);
			myconfig.permitted_mimes.erase(mime);
			myconfig.save();
			return;
		}
	}
	printf(_("\"%s\" was not found in the list of permitted MIME types\n"),optarg);

}

void help(void)
{

	printf(_("Usage: tuxcast-config  [action]\n"));
	printf(_("Actions are:\n"));
	printf(_("-a -n NAME -A ADDRESS -f FOLDER: Add a feed with the specified name, address and folder\n"));
	printf(_("-d NAME: Delete the feed with the specified name\n"));
	printf(_("-g OPTION: Get the value of the specified option\n"));
	printf(_("-g feed -n NAME: Get the settings of the specified feed\n"));
	printf(_("-G: Get the value of all options\n"));
	printf(_("-h: Display this help message\n"));
	printf(_("-s: OPTION=VALUE: Set OPTION to VALUE\n"));
	printf(_("-m: List permitted MIME types\n"));
	printf(_("-t TYPE: Allow a new mime type\n"));
	printf(_("-r TYPE: Reject a mime type\n"));
	printf(_("-u -n NAME [ -A ADDRESS | -f FOLDER | -N NEWNAME | -p PATTERN ]: Change a property of a feed\n\n"));
	printf(_("Available options are:\n"));
	printf(_("podcastdir - where to save all your podcasts\n"));
	printf(_("ask - whether or not to ask before downloading any files\n"));
	printf(_("threads - number of download threads to spawn\n"));
}

void get(int argc, char **argv)
{
	string args = optarg; // This is the first option's optarg - although
	// getopt was called ageeees ago we can still retrieve optarg here.

	if(strcasecmp(args.c_str(),"podcastdir") == 0)
	{
		printf("%s\n",myconfig.podcastdir.c_str()); // No translation
		// required for just a format string, I spose.
		return;
	}
#ifdef THREADS
	if(strcasecmp(args.c_str(), "threads") == 0)
	{
		printf("%d\n", myconfig.numofdownloaders);
		return;
	}
#endif

	if(strcasecmp(args.c_str(), "postdownload") == 0)
	{
		printf("%s\n", myconfig.postdownload.c_str());
		return;
	}
	if(strcasecmp(args.c_str(), "postrun") == 0)
	{
		printf("%s\n", myconfig.postrun.c_str());
		return;
	}
	if(strcasecmp(args.c_str(), "incorrectmime") == 0)
	{
		printf("%s\n", myconfig.incorrectmime.c_str());
		return;
	}

	if(strcasecmp(args.c_str(),"ask") == 0)
	{
		if(myconfig.ask == true)
			printf(_("true"));
		else
			printf(_("false"));
		return;
	}
	// DANGER DANGER WILL ROBINSON!!1  MY ARMS ARE FLAILING WILDLY!!!11one
	// If the thing after -g is shorter than 4 chars we could have a problem
	if(strcasecmp(args.c_str(),"feed") == 0)
	{
		if(getopt(argc,argv,options) != 'n')
		{
			fprintf(stderr,_("You must pass a feed name with -n NAME\n"));
			return;
		}
		string name = optarg;
		// Locate which feed they are talking about:
		FOREACH(configuration::feedlist::iterator, myconfig.feeds, feed)
		{
			if(strcasecmp(feed->name.c_str(),name.c_str()) == 0) // Found it:
			{
				feed->displayConfig();
				return;
			}
			else
				continue; // RRRAAAWWWRRRR.  :P
		}
		// If we get here the feed name is bollocks:
		fprintf(stderr,_("Invalid feed name, %s\n"),name.c_str());
		return;
	}
	if(strcasecmp(args.c_str(),"feeds") == 0)
	{
		// Display all feeds:
		FOREACH(configuration::feedlist::iterator, myconfig.feeds, feed)
		{
			feed->displayConfig();
		}
		return;

	}
	// other variables go here...
	
	// If we are still here and haven't returned, somebody messed up.
	// Probably the user ;)
	fprintf(stderr,_("Unrecognised option, %s\n"),args.c_str());
}

void getall(void)
{
	printf(_("podcastdir = %s\n"),myconfig.podcastdir.c_str());
	printf(_("ask = %s\n"), (myconfig.ask?"true":"false"));
#ifdef THREADS
	printf(_("threads = %d\n"), myconfig.numofdownloaders);
#endif

	printf(_("postdownload = %s\n"), myconfig.postdownload.c_str());
	printf(_("postrun = %s\n"), myconfig.postrun.c_str());
	printf(_("incorrectmime = %s\n"), myconfig.incorrectmime.c_str());

	listmimes();
	// ...
	// Let's show some feeds:
	printf(ngettext("There is 1 feed:\n", "There are %d feeds:\n",
				myconfig.feeds.size()),myconfig.feeds.size());
        map<string,int> dirs;
        int id = 0;
        int max_width = 0;
        // 1. get mapping between id and feed folder
        // 2. get max feed name width
	FOREACH(configuration::feedlist::iterator, myconfig.feeds, feed)
	{
            if (dirs.find(feed->folder) == dirs.end())
            {
                dirs[feed->folder] = id;
                printf(_("Folder: %s = %d\n"), feed->folder.c_str(), id);
                id++;
            }
            max_width = MAX(max_width, feed->name.size());
	}
	FOREACH(configuration::feedlist::iterator, myconfig.feeds, feed)
	{
		feed->displayConfig(max_width, dirs);
	}

}

void set()
{
	string args = optarg;
	string varname=args.substr(0,args.find("=",0));
	string value=args.substr(args.find("=",0)+1,args.length()-args.find("=",0)-1);

	if(strcasecmp(varname.c_str(),"podcastdir") == 0)
		myconfig.podcastdir = value;

#ifdef THREADS
	if(strcasecmp(varname.c_str(), "threads") == 0)
		myconfig.numofdownloaders = atoi(value.c_str());
#endif

	if(strcasecmp(varname.c_str(), "postdownload") == 0)
		myconfig.postdownload = value;
	if(strcasecmp(varname.c_str(), "postrun") == 0)
		myconfig.postrun = value;
	if(strcasecmp(varname.c_str(), "incorrectmime") == 0)
		myconfig.incorrectmime = value;

	if(strcasecmp(varname.c_str(),"ask") == 0)
	{
		if((strcasecmp(value.c_str(),"true") == 0) || (strcasecmp(value.c_str(),"yes") == 0))
		{
			myconfig.ask = true;
		}
		else
		{
			if((strcasecmp(value.c_str(),"false") == 0) || (strcasecmp(value.c_str(),"no") == 0))
			{
				myconfig.ask = false;
			}
			else
			{
				fprintf(stderr,_("The \"ask\" variable can only be \"true\" or \"false\" -\nAssuming true\n"));
				myconfig.ask = true;
			}
		}
	}
	// ...
	
	myconfig.save();
}

void add(int argc,char *argv[])
{
	int myopt=0;
	string name, address, folder;

	while((myopt = getopt(argc,argv,options)) != -1)
	{
		if((char)myopt == 'A')
			address = optarg;
		if((char)myopt == 'n')
			name = optarg;
		if((char)myopt == 'f')
			folder = optarg;
	}

	if(name.empty())
	{
		fprintf(stderr,_("No name specified!\n"));
		return;
	}
	if(address.empty())
	{
		fprintf(stderr, _("No URL specified!\n"));
		return;
	}
	// folder can be blank - it means just put the podcast in podcastdir

	FOREACH(configuration::feedlist::iterator, myconfig.feeds, feed)
	{
		// Let's check the feed isn't already there:
		if(strcasecmp(feed->name.c_str(),name.c_str()) == 0)
		{
			fprintf(stderr,_("Feed already exists\n"));
			return;
		}
		// Check the URL isn't a duplicate:
		if(strcasecmp(feed->address.c_str(),address.c_str()) == 0)
		{
			fprintf(stderr, _("A feed with the same URL already exists\n"));
			return;
		}
	}

	myconfig.feeds.push_back(feed(name, address, folder));
	
	myconfig.save();
}

void del()
{
	FOREACH(configuration::feedlist::iterator, myconfig.feeds, feed)
	{
		if(strcasecmp(optarg,feed->name.c_str()) == 0)
		{
			string ans;
			printf(_("Are you sure you wish to delete the feed \"%s\"? (yes|no)\n"), feed->name.c_str());
			cin >> ans;
			if(strcasecmp(ans.c_str(),"yes") != 0)
				return;

			// We've found the feed - wipe it:
			myconfig.feeds.erase(feed);
			myconfig.save();
			return;
		}
	}
	fprintf(stderr,_("Feed \"%s\" doesn't exist\n"),optarg);
}
	
void update(int argc, char *argv[])
{
	char myopt;
	string name;
	// The -u option was already sucked up
	// The next option must be -n so we know what to update
	// After that, we need either -f or -A
	
	myopt = getopt(argc,argv,options);
	if(myopt != 'n')
	{
		fprintf(stderr,_("You must pass -n and either -A, -f, -N or -p when updating a feed's info\n"));
		return;
	}
	name = optarg;
	myopt = getopt(argc,argv,options);
		
	FOREACH(configuration::feedlist::iterator, myconfig.feeds, feed)
	{
		if(strcasecmp(feed->name.c_str(),name.c_str()) == 0)
		{
			// Found the feed, update it:
			switch(myopt)
			{
				case 'A':
					if(strcmp(optarg,"") == 0)
					{
						fprintf(stderr,_("Error: Blank address\n"));
						return;
					}
					FOREACH(configuration::feedlist::iterator, myconfig.feeds, feed_finder)
					{
						if(strcmp(feed_finder->address.c_str(), optarg) == 0)
						{
							fprintf(stderr,_("Error: Another feed already exists with this address\n"));
							return;
						}
					}
					
					feed->address = optarg;
					break;

				case 'f':
					feed->folder = optarg;
					break;
					
				case 'N':
					if(strcmp(optarg,"") == 0)
					{
						fprintf(stderr,_("Error: Blank name\n"));
						return;
					}
					
					FOREACH(configuration::feedlist::iterator, myconfig.feeds, feed_finder)
					{
						if(strcmp(feed_finder->name.c_str(), optarg) == 0)
						{
							fprintf(stderr,_("Error: Another feed already exists with this name\n"));
							return;
						}
					}
					
					feed->name=optarg;
					break;
					
				case 'p':
                                        feed->exclude_pats.push_back(optarg);
					break;
					
				default:
					fprintf(stderr,_("You must pass either -A, -f, -N or -p when updating a feed's info\n"));
					return;
			}

			// We've found the feed and updated it
			myconfig.save();
			return;
		}
	}
fprintf(stderr,_("Error: feed \"%s\" doesn't exist.\n"),name.c_str());
return;
}
