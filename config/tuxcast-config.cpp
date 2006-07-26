#include "../compile_flags.h"
#include <iostream>
#include "config.h"
#include <unistd.h>
#include "config_exceptions.h"
#include "../libraries/filestuff_exceptions.h"
// Don't need operations.hpp in here (yet)

const char options[] = "aA:d:n:hs:g:Gf:u";

configuration myconfig;


using namespace std;

void help(void);
void get(string args);
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
				get(optarg);
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

			default:
				cerr << "Error: You must pass either -a, -s, -g, -G or -h" << endl;
				cerr << "Pass -h for help" << endl;
		}
	}
	catch(eFilestuff_CannotCreateFolder &e)
	{
		cerr << "Cannot save config file, because I cannot create the ~/.tuxcast folder" << endl;
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
	cout << "-G: Get the value of all options" << endl;
	cout << "-h: Display this help message" << endl;
	cout << "-s OPTION=VALUE: Set OPTION to VALUE" << endl;
	cout << "-u -n NAME [ -A ADDRESS | -f FOLDER ]" << endl;
	cout << endl;
	cout << "Available options are:" << endl;
	cout << "podcastdir - where to save all your podcasts" << endl;

}

void get(string args)
{
	if(strcmp(args.c_str(),"podcastdir") == 0)
		cout << myconfig.podcastdir << endl;
	if(strcmp(args.c_str(),"ask") == 0)
	{
		if(myconfig.ask == true)
			cout << "true" << endl;
		else
			cout << "false" << endl;
	}

	// other variables go here...
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
		cout << "Folder: " << myconfig.feeds[i]->folder << endl;
		cout << "--" << endl;
	}

}

void set(string args)
{
	string varname=args.substr(0,args.find("=",0));
	string value=args.substr(args.find("=",0)+1,args.length()-args.find("=",0)-1);

	if(strcmp(varname.c_str(),"podcastdir") == 0)
	{
		myconfig.podcastdir = value;
	}
	if(strcmp(varname.c_str(),"ask") == 0)
	{
		if(strcmp(value.c_str(),"true") == 0)
		{
			myconfig.ask = true;
		}
		else
		{
			if(strcmp(value.c_str(),"false") == 0)
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
		if(strcmp(myconfig.feeds[i]->name.c_str(),name.c_str()) == 0)
		{
			cerr << "Feed already exists" << endl;
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
		cerr << "You must pass both the name and address of the feed to add" << endl;
	
	myconfig.save();
}

void del(string name)
{
	int i=0,j=0;
	vector<feed *>::iterator myiterator;
	
	while(true)
	{
		if(strcmp(name.c_str(),myconfig.feeds[i]->name.c_str()) == 0)
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
		if(strcmp(myconfig.feeds[i]->name.c_str(), name.c_str()) == 0)
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
		cerr << "You must pass -n and either -A or -f, when updating a feed's info" << endl;
		return;
	}
	name = optarg;
	myopt = getopt(argc,argv,options);
		
	while(true)
	{
		if(strcmp(myconfig.feeds[i]->name.c_str(),name.c_str()) == 0)
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
					
					myconfig.feeds[i]->address = optarg;
					break;

				case 'f':
					myconfig.feeds[i]->folder = optarg;
					break;
				default:
					cerr << "You must pass either -A or -f, when updating a feed's info" << endl;
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
