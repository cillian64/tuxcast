#include "../compile_flags.h"
#include <iostream>
#include "config.h"
#include <unistd.h>
#include <boost/filesystem/path.hpp>
// Don't need operations.hpp in here (yet)

const char options[] = "aA:d:n:hs:g:Gf:";

configuration myconfig;


using namespace std;

void help(void);
void get(string args);
void getall(void);
void set(string args);
void add(int argc,char *argc[]);
void del(string name);
void BREAK(){}

int main(int argc, char *argv[])
{
	try
        {
#ifdef POSIX
                boost::filesystem::path::default_name_check(boost::filesystem::portable_posix_name);
#endif
#ifdef WINDOWS
                boost::filesystem::path::default_name_check(boost::filesystem::windows_name);
#endif
        }
        catch(...)
        {       
                // The only reason default_name_check should fail is if
		// You have run it twice - this could be caused by:
                cerr << "ERROR: you have both WINDOWS and POSIX compile flags enabled." << endl;
                cerr << "If you compiled manually, check your compile_flags.h" << endl;
                cerr << "Else, contact your package maintainer" << endl;
        }
	
	myconfig.load();
	
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

		case 'h':
			help();	
			break;

		default:
			cerr << "Error: You must pass either -a, -s, -g, -G or -h" << endl;
			cerr << "Pass -h for help" << endl;
	}
	
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
	if(myconfig.numoffeeds == 1)
		cout << "There is 1 feed:" << endl;
	else
		cout << "There are " << myconfig.numoffeeds << " feeds:" << endl;
	for(int i=0; i<myconfig.numoffeeds; i++)
	{
		cout << "Name: " << myconfig.feeds[i].name << endl;
		cout << "Address: " << myconfig.feeds[i].address << endl;
		cout << "Folder: " << myconfig.feeds[i].folder << endl;
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
	int myopt=0;
	string name, address, folder;
	feed *newarray;

	while((myopt = getopt(argc,argv,options)) != -1)
	{
		if((char)myopt == 'A')
			address = optarg;
		if((char)myopt == 'n')
			name = optarg;
		if((char)myopt == 'f')
			folder = optarg;
	}

	if((strcmp(name.c_str(),"") != 0) && (strcmp(address.c_str(),"") != 0))
	{
		newarray = new feed[myconfig.numoffeeds+1];
		for(int i=0; i<myconfig.numoffeeds;i++)
		{
			newarray[i].name = myconfig.feeds[i].name;
			newarray[i].address = myconfig.feeds[i].address;
			newarray[i].folder = myconfig.feeds[i].folder;
			// Copy over all old feeds' data
		}
		// Add the new feed:
		newarray[myconfig.numoffeeds].name = name;
		newarray[myconfig.numoffeeds].address = address;
		newarray[myconfig.numoffeeds].folder = folder;

		
		// Finally, delete the old array of feeds, and swap in the new array of feeds, and update numoffeeds
		delete[] myconfig.feeds;
		myconfig.feeds = newarray;
		myconfig.numoffeeds++;
	}
	else
		cerr << "You must pass both the name and address of the feed to add" << endl;
	
	myconfig.save();
}

void del(string name)
{
	feed *newarray;
	int j=0;
	newarray = new feed[myconfig.numoffeeds-1];

	for(int i=0; i<myconfig.numoffeeds; i++)
	{
		if(strcmp(myconfig.feeds[i].name.c_str(),name.c_str()) != 0)
		{
			newarray[j].name = myconfig.feeds[i].name;
			newarray[j].address = myconfig.feeds[i].address;
			newarray[j].folder = myconfig.feeds[i].folder;
			j++;
		}
	}
	delete[] myconfig.feeds;
	BREAK();
	myconfig.feeds = newarray;
	myconfig.numoffeeds--;
	myconfig.save();
}
	


