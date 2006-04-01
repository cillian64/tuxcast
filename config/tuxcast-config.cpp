#include <iostream>
#include "config.h"
#include <unistd.h>

const char options[] = "hs:g:G";

configuration myconfig;


using namespace std;

void help(void);
void get(string args);
void getall(void);
void set(string args);

int main(int argc, char *argv[])
{
	myconfig.load("config.xml");
	
	switch(getopt(argc,argv,options))
	{
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
			cout << "Error: You must pass either -s, -g, -G or -h" << endl;
	}
	
	return 0;
}

void help(void)
{
	cout << "tuxcast-config [action]" << endl;
	cout << "Actions are:" << endl;
	cout << "-s OPTION=VALUE: Set OPTION to VALUE" << endl;
	cout << "-g OPTION: Get the value of option" << endl;
	cout << "-G: Get the value of all options" << endl;
	cout << "-h: Display this help message" << endl;
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

