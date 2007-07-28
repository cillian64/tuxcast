#include <iostream>
#include <string>
#include <vector>
#include "rss.h"
#include "config.h"

using namespace std;

int main()
{
	configuration *myconfig = new configuration;
	vector<string*> *downloads;

	cout << "Hello, world!" << endl;
	cout << "Loading up configuration, default location..." << endl;
	myconfig->load();
	cout << "Initializing filename list..." << endl;
	downloads = new vector<string*>;
	cout << "Beginning iteration through feeds..." << endl;
	for(unsigned int i=0; i<myconfig->feeds.size(); i++)
	{
		filelist *files;
		cout << "Checking feed " << i << ", named " << myconfig->feeds[i]->name << "." << endl;
		try
		{
			string path=getenv("HOME");
			path+="/.tuxcast/cache/";
			path+=myconfig->feeds[i]->name;

			files = parse(path);
		}
		catch( ... )
		{
			cerr << "Yeh, something fucked up." << endl;
			continue;
		}
		if(files == NULL)
		{
			cerr << "WTF?!" << endl;
			continue;
		}
		cout << "Got " << files->size() << " files." << endl;
		cout << "Exporting..." << endl;
		for(unsigned int j=0; j<files->size(); j++)
		{
			string *newfile = new string;
			*newfile = (*files)[j]->filename;
			downloads->push_back(newfile);
			cout << "Exported \"" << (*files)[j]->filename << "\"." << endl;
		}
		cout << "Exported " << files->size() << " files." << endl;
		cout << "Clearing up..." << endl;
		for(unsigned int j=0; j<files->size(); j++)
			delete (*files)[j];
		delete files;
	}
	cout << "Got a total of " << downloads->size() << " files." << endl;


	return 0;
}
