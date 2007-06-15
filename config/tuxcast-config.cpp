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
#include "config.h"
#include <unistd.h>
#include "config_exceptions.h"
#include "../libraries/filestuff_exceptions.h"
#include "../version.h"

const char options[] = "aA:d:n:N:hs:g:Gf:uv";

configuration myconfig;


using namespace std;

void help(void);
void get(int argc, char **argv);
void getall(void);
void set(string args);
void add(int argc,char *argc[]);
void del(string name);
void update(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	try
	{
		myconfig.load();
	}
	catch(eConfig_NoConfigFile &e)
	{
		cerr << "Warning, No config file" << endl;
		cerr << "If you're changing an option or adding a feed, a config file will be created later" << endl;
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
		cerr << "Cannot save config file" << endl;
		cerr << "Exception caught: ";
		e.print();
		return -1;
	}
	// No point in catching that exception separatly in every action
	// Just catch here to avoid SIGABRT and exit cleanly
		
	return 0;
}

void help(void)
{
	cout << "tuxcast-config [action]" << endl;
	cout << "Actions are:" << endl;
	cout << "-a -n NAME -A ADDRESS -f FOLDER: Add a feed with the specified name, address and folder" << endl;
	cout << "-d NAME: Delete the feed with the specified name" << endl;
	cout << "-g OPTION: Get the value of option" << endl;
	cout << "-g feed -n NAME: Get the settings of a particular feed, FEED" << endl;
	cout << "-g feeds: Show all feeds" << endl;
	cout << "-G: Get the value of all options" << endl;
	cout << "-h: Display this help message" << endl;
	cout << "-s OPTION=VALUE: Set OPTION to VALUE" << endl;
	cout << "-u -n NAME [ -A ADDRESS | -f FOLDER | -N NEWNAME ]: Change a property of a feed" << endl;
	cout << endl;
	cout << "Available options are:" << endl;
	cout << "podcastdir - where to save all your podcasts" << endl;
	cout << "ask - whether or not to ask before downloading every episode" << endl;

}

void get(int argc, char **argv)
{
	string args = optarg; // This is the first option's optarg - although
	// getopt was called ageeees ago we can still retrieve optarg here.

	if(strcasecmp(args.c_str(),"podcastdir") == 0)
	{
		cout << myconfig.podcastdir << endl;
		return;
	}
	if(strcasecmp(args.c_str(),"ask") == 0)
	{
		if(myconfig.ask == true)
			cout << "true" << endl;
		else
			cout << "false" << endl;
		return;
	}
	// DANGER DANGER WILL ROBINSON!!1  MY ARMS ARE FLAILING WILDLY!!!11one
	// If the thing after -g is shorter than 4 chars we could have a problem
	if(strcasecmp(args.c_str(),"feed") == 0)
	{
		if(getopt(argc,argv,options) != 'n')
		{
			cerr << "You must pass a feed name with -n NAME" << endl;
			return;
		}
		string name = optarg;
		// Locate which feed they are talking about:
		for(int i=0; i<myconfig.feeds.size(); i++)
		{
			if(strcasecmp(myconfig.feeds[i]->name.c_str(),name.c_str()) == 0) // Found it:
			{
				cout << "Name: " << myconfig.feeds[i]->name.c_str() << endl;
				cout << "Address: " << myconfig.feeds[i]->address.c_str() << endl;
				cout << "Folder: " << myconfig.feeds[i]->folder.c_str() << endl;
				return;
			}
			else
				continue; // RRRAAAWWWRRRR.  :P
		}
		// If we get here the feed name is bollocks:
		cerr << "Invalid feed name, " << name << endl;
		return;
	}
	if(strcasecmp(args.c_str(),"feeds") == 0)
	{
		// Display all feeds:
		for(int i=0; i<myconfig.feeds.size(); i++)
		{
			cout << "Name: " << myconfig.feeds[i]->name.c_str() << endl;
			cout << "Address: " << myconfig.feeds[i]->address.c_str() << endl;
			cout << "Folder: " << myconfig.feeds[i]->folder.c_str() << endl << endl;
		}
		return;

	}
	// other variables go here...
	//
	
	// If we are still here and haven't returned, somebody messed up.
	cerr << "Unrecognised option, " << args << endl;
}

void getall(void)
{
	cout << "podcastdir = " << myconfig.podcastdir << endl;
	if(myconfig.ask == true)
	{
		cout << "ask = true" << endl;
	}
	else
		cout << "ask = false" << endl;
	
	// ...
	// Let's show some feeds:
	if(myconfig.feeds.size() == 1)
		cout << "There is 1 feed:" << endl;
	else // Yay for bulky code for the sake of good grammar :-)
		cout << "There are " << myconfig.feeds.size() << " feeds:" << endl;
	for(int i=0; i<myconfig.feeds.size(); i++)
	{
		cout << "Name: " << myconfig.feeds[i]->name << endl;
		cout << "Address: " << myconfig.feeds[i]->address << endl;
		cout << "Folder: " << myconfig.feeds[i]->folder << endl << endl;
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
				cerr << "The \"ask\" variable can only be \"true\" or \"false\"" << endl;
				cerr << "Assuming true" << endl;
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
		cerr << "No name specified!!" << endl;
		return;
	}
	while(i<myconfig.feeds.size())
	{
		if(strcasecmp(myconfig.feeds[i]->name.c_str(),name.c_str()) == 0)
		{
			cerr << "Feed already exists" << endl;
			return;
		}
		else
			i++;
	}
	
	// Check the URL isn't a duplicate:
	// Sanity check:
	if(strcmp(address.c_str(),"") == 0)
	{
		cerr << "No URL specified!" << endl;
		return;
	}
	i=0;
	while(i<myconfig.feeds.size())
	{
			if(strcasecmp(myconfig.feeds[i]->address.c_str(),address.c_str()) == 0)
			{
				cerr << "A feed with the same URL already exists" << endl;
				return;
			}
			else
				i++;
	}	
	// TODO: Stick those two above together for neatness.
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
		cerr << "You must pass both the name and address of the feed to add" << endl;
	
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
		
		cerr << "Feed \"" << name << "\" doesn't exist" << endl;
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
		cerr << "You must pass -n and either -A, -f or -N when updating a feed's info" << endl;
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
						cerr << "Error: Blank address" << endl;
						return;
					}
					for(int j=0; j<myconfig.feeds.size(); j++)
					{
						if(strcmp(myconfig.feeds[j]->address.c_str(), optarg) == 0)
						{
							cerr << "Error: Another feed already exists with this address" << endl;
							return;
						}
					}
					
					myconfig.feeds[i]->address = optarg;
					break;

				case 'f':
					myconfig.feeds[i]->folder = optarg;
					break;
					
				case 'N':
					if(strcmp(optarg,"") == 0)
					{
						cerr << "Error: Blank name" << endl;
						return;
					}
					
					for(int j=0; j<myconfig.feeds.size(); j++)
					{
						if(strcmp(myconfig.feeds[j]->name.c_str(), optarg) == 0)
						{
							cerr << "Error: Another feed already exists with this name" << endl;
							return;
						}
					}
					
					myconfig.feeds[i]->name=optarg;
					break;
					
				default:
					cerr << "You must pass either -A, -f or -N when updating a feed's info" << endl;
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
		cerr << "Error: feed \"" << name << "\" doesn't exist." << endl;
		return;
	}
	myconfig.save();
}
