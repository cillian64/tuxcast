#include <iostream>
#include "rss.h"
#include "../config/config.h"
#include <curl/curl.h>
#include <cstdio> // Needed to open a file in the classic way, so libcurl can write to it

using namespace std;

int main(void)
{
	filelist *myfilelist;
	string temp;
	FILE *outputfile;
	CURL *mycurl;
	configuration myconfig;
	mycurl = curl_easy_init();
	if(mycurl == NULL)
	{
		cerr << "Error initializing libcurl" << endl;
		return -1;
	}
	
	myconfig.load();
	for(int i=0; i<myconfig.numoffeeds; i++)
	{
		cout << "Checking feed \"" << myconfig.feeds[i].name << "\"..." << endl;
		myfilelist = parse(myconfig.feeds[i].address);
		for(int j=0, size=myfilelist->numoffiles(); j<size; j++)
		{
			cout << "File found..." << endl;
			cout << "Name is " << myfilelist->getfilename(j) << endl;
			cout << "Download?: (yes/no)" << endl;
			cin >> temp;
			if(strcmp(temp.c_str(),"yes") == 0)
			{
				cout << "Downloading..." << endl;
				outputfile = fopen(myfilelist->getfilename(j).c_str(), "w");
				if(outputfile == NULL)
				{
					cerr << "Error opening output file" << endl;
				}
				curl_easy_setopt(mycurl,CURLOPT_URL,myfilelist->getURL(j).c_str());
				curl_easy_setopt(mycurl,CURLOPT_WRITEDATA,outputfile);
				curl_easy_perform(mycurl);
				fclose(outputfile);
				cout << "Done." << endl;
			}
		}
	}
	
	curl_easy_cleanup(mycurl);
	
	return 0;
}

