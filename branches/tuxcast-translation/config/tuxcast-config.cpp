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
#include "config.h"
#include <unistd.h>
#include "config_exceptions.h"
#include "../libraries/filestuff_exceptions.h"
#include "../version.h"
#include <libintl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

#define _(x) gettext(x)

const char options[] = "aA:d:n:hs:g:Gf:uv";

configuration myconfig;

void help(void);
void get(int argc, char **argv);
void getall(void);
void set(string args);
void add(int argc,char *argc[]);
void del(string name);
void update(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	// TODO: Insert internationalisation setup shizzle here:
	setlocale(LC_ALL,"");
	bindtextdomain("tuxcast","/usr/share/locale");
	textdomain("tuxcast");
	
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
				del(optarg);
				break;
			
			case 'g':
				get(argc,argv); // Get needs to use the options
				// for a few things.
				break;

			case 'G':
				getall();
				break;

			case 's':
				set(optarg);
				break;
			case 'u':
				update(argc,argv);
				break;

			case 'h':
				help();	
				break;

			case 'v':
				version();
				break;
		
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
	printf(_("-s OPTION=VALUE: Set OPTION to VALUE\n"));
	printf(_("-u -n NAME [ -A ADDRESS | -f FOLDER ]: Change the specified feed's name or address to the specified value\n\n"));
	printf(_("Available options are:\n"));
	printf(_("podcastdir - where to save all your podcasts\n"));
	printf(_("ask - whether or not to ask before downloading any files\n"));

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
		for(int i=0; i<myconfig.feeds.size(); i++)
		{
			if(strcasecmp(myconfig.feeds[i]->name.c_str(),name.c_str()) == 0) // Found it:
			{
				printf(_("Name: %s\n"),myconfig.feeds[i]->name.c_str());
				printf(_("Address: %s\n"),myconfig.feeds[i]->address.c_str());
				printf(_("Folder: %s\n"),myconfig.feeds[i]->folder.c_str());
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
		for(int i=0; i<myconfig.feeds.size(); i++)
		{
			printf(_("Name: %s\n"),myconfig.feeds[i]->name.c_str());
			printf(_("Address: %s\n"),myconfig.feeds[i]->address.c_str());
			printf(_("Folder: %s\n"),myconfig.feeds[i]->folder.c_str());
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
	if(myconfig.ask == true)
		printf(_("ask = true\n"));
	else
		printf(_("ask = false\n"));
	
	// ...
	// Let's show some feeds:
	if(myconfig.feeds.size() == 1)
		printf(_("There is 1 feed:\n"));
	else // Yay for bulky code for the sake of good grammar :-)
		printf(_("There are %d feeds:\n"),myconfig.feeds.size());
	for(int i=0; i<myconfig.feeds.size(); i++)
	{
		printf(_("Name: %s\n"),myconfig.feeds[i]->name.c_str());
		printf(_("Address: %s\n"),myconfig.feeds[i]->address.c_str());
		printf(_("Folder: %s\n"),myconfig.feeds[i]->folder.c_str());
	}

}

void set(string args)
{
	string varname=args.substr(0,args.find("=",0));
	string value=args.substr(args.find("=",0)+1,args.length()-args.find("=",0)-1);

	if(strcasecmp(varname.c_str(),"podcastdir") == 0)
	{
		myconfig.podcastdir = value;
	}
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
	int myopt=0, i=0;
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

	// Let's check the feed isn't already there:
	if(strcmp(name.c_str(),"") == 0)
	{
		fprintf(stderr,_("No name specified!\n"));
		return;
	}
	while(i<myconfig.feeds.size())
	{
		if(strcasecmp(myconfig.feeds[i]->name.c_str(),name.c_str()) == 0)
		{
			fprintf(stderr,_("Feed already exists\n"));
			return;
		}
		else
			i++;
	}
		
	
	if((strcmp(name.c_str(),"") != 0) && (strcmp(address.c_str(),"") != 0)) // No sanity check for folder here:
		// folder can be blank - it means just put the podcast in podcastdir
	{
		myconfig.feeds.push_back(NULL);
		myconfig.feeds[myconfig.feeds.size()-1] = new feed;
		myconfig.feeds[myconfig.feeds.size()-1]->name = name;
		myconfig.feeds[myconfig.feeds.size()-1]->address = address;
		myconfig.feeds[myconfig.feeds.size()-1]->folder = folder;
	}
	else
		fprintf(stderr,_("You must pass the name and address of the feed to add (The folder can be left blank to just put files in podcastdir)\n"));
	
	myconfig.save();
}

void del(string name)
{
	int i=0,j=0;
	vector<feed *>::iterator myiterator;
	
	while(true)
	{
		if(strcasecmp(name.c_str(),myconfig.feeds[i]->name.c_str()) == 0)
			break; // Found the feed to delete, we're good

		if(i<myconfig.feeds.size()-1) // if i is 1 less than numoffeeds, we're
			// at the end of the array, so we don't want to loop again:
			// if we're at the end of the array and didn't just break,
			// the feed doesn't exist
		{
			i++;
			continue;
		}

		// If we get here, we're at the end of the array (we /should/ have
		// either broken or continued if the feed exists anywhere
		
		fprintf(stderr,_("Feed \"%s\" doesn't exist\n"),name.c_str());
		return;
	}
	
	for(int i=0; i<myconfig.feeds.size(); i++)
	{
		if(strcasecmp(myconfig.feeds[i]->name.c_str(), name.c_str()) == 0)
		{
			// We've found the feed - wipe it:
			myiterator = myconfig.feeds.begin();
			myiterator += i;
			myconfig.feeds.erase(myiterator,myiterator+1);
			// Stupid STL functions not taking subscripts
			// >:-(
		}
	}
	// Yup, that really is all - yay for STL vectors
	
	myconfig.save();
}
	
void update(int argc, char *argv[])
{
	int i=0;
	char myopt;
	string name;
	// The -u option was already sucked up
	// The next option must be -n so we know what to update
	// After that, we need either -f or -A
	
	myopt = getopt(argc,argv,options);
	if(myopt != 'n')
	{
		fprintf(stderr,_("You must pass -n and either -A or -f when updating a feed's info\n"));
		return;
	}
	name = optarg;
	myopt = getopt(argc,argv,options);
		
	while(true)
	{
		if(strcasecmp(myconfig.feeds[i]->name.c_str(),name.c_str()) == 0)
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
					
					myconfig.feeds[i]->address = optarg;
					break;

				case 'f':
					myconfig.feeds[i]->folder = optarg;
					break;
				default:
					fprintf(stderr,_("You must pass either -A or -f when updating a feed's info\n"));
					return;
			}
			break;
		}

		if(i<myconfig.feeds.size()-1)
		{
			i++;
			continue;
		}
		// This means i is 1 less than numoffeeds
		// We still haven't found the feed
		// This means the feed aint there
		fprintf(stderr,_("Error: feed \"%s\" doesn't exist.\n"),name.c_str());
		return;
	}
	myconfig.save();
}
