#include <iostream>
#include "config.h"
#include <unistd.h>

const char options[] = "aA:n:hs:g:G";

configuration myconfig;


using namespace std;

void help(void);
void get(string args);
void getall(void);
void set(string args);
void add(int argc,char *argc[]);

int main(int argc, char *argv[])
{
	myconfig.load("config.xml");
	
	switch(getopt(argc,argv,options))
	{
		case 'a':
			add(argc,argv);
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
	cout << "-a -n NAME -A ADDRESS: Add a feed with the specified name & address" << endl;
	cout << "-g OPTION: Get the value of option" << endl;
	cout << "-G: Get the value of all options" << endl;
	cout << "-h: Display this help message" << endl;
	cout << "-s OPTION=VALUE: Set OPTION to VALUE" << endl;

}

void get(string args)
{
	if(strcmp(args.c_str(),"podcastdir") == 0)
		cout << myconfig.podcastdir << endl;
	// other variables go here...
}

void getall(void)
{
	cout << "podcastdir = " << myconfig.podcastdir << endl;
	// ...
	// Let's show some feeds:
	cout << "There are " << myconfig.numoffeeds << " feeds:" << endl;
	cout << "Feeds:" << endl;
	for(int i=0; i<myconfig.numoffeeds; i++)
	{
		cout << "Name: " << myconfig.feeds[i].name << endl;
		cout << "Address: " << myconfig.feeds[i].address << endl;
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
	// ...
	
	myconfig.save("config.xml");
}

void add(int argc,char *argv[])
{
	int myopt=0;
	string name, address;
	feed *newarray;
	cout << "Adding" << endl;

	while((myopt = getopt(argc,argv,options)) != -1)
	{
		if((char)myopt == 'A')
		{
			cout << "Address is " << optarg << endl;
			address = optarg;
		}
		if((char)myopt == 'n')
		{
			cout << "Name is " << optarg << endl;
			name = optarg;
		}
	}

	if((strcmp(name.c_str(),"") != 0) && (strcmp(address.c_str(),"") != 0))
	{
		newarray = new feed[myconfig.numoffeeds+1];
		for(int i=0; i<myconfig.numoffeeds;i++)
		{
			newarray[i].name = myconfig.feeds[i].name;
			cout << "Old name is " << myconfig.feeds[i].name;
			cout << "New name is " << newarray[i].name;
			cout << "i is " << i << endl;
			newarray[i].address = myconfig.feeds[i].address;
			// Copy over all old feeds' data
		}
		// Add the new feed:
		newarray[myconfig.numoffeeds].name = name;
		newarray[myconfig.numoffeeds].address = address;
		
		// Finally, delete the old array of feeds, and swap in the new array of feeds, and update numoffeeds
		delete[] myconfig.feeds;
		myconfig.feeds = newarray;
		myconfig.numoffeeds++;
	}
	else
		cerr << "You must pass both the name and address of the feed to add" << endl;
	
	getall(); // To demo if the feed was added

	
	cout << "Done" << endl;

}
