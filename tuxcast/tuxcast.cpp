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
	mycurl = curl_easy_init();
	if(mycurl == NULL)
	{
		cerr << "Error initializing libcurl" << endl;
		return -1;
	}
	
	myfilelist = parse("http://www.lugradio.org/episodes.rss"); // parse takes a filelist pointer, makes a new filelist, and fills it

	for(int i=0, size=myfilelist->numoffiles(); i<size; i++)
	{
		cout << "File(s) found..." << endl;
		cout << "name is " << myfilelist->getfilename(i) << endl;
		cout << "length is " << myfilelist->getlength(i) << endl;
		cout << "URL is " << myfilelist->getURL(i) << endl;
		cout << "Download?: (yes/no)" << endl;
		cin >> temp;
		if(strcmp(temp.c_str(),"yes") == 0)
		{
			cout << "Downloading..." << endl;
			outputfile = fopen(myfilelist->getfilename(i).c_str(), "w");
			if(outputfile == NULL)
			{
				cerr << "Error opening output file" << endl;
			}
			curl_easy_setopt(mycurl,CURLOPT_URL,myfilelist->getURL(i).c_str());
			curl_easy_setopt(mycurl,CURLOPT_WRITEDATA,outputfile);
			curl_easy_perform(mycurl);
			fclose(outputfile);
		}
	}
	
	curl_easy_cleanup(mycurl);
	
	return 0;
}

