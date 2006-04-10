#include <iostream>
#include "rss.h"
#include "../config/config.h"
#include <curl/curl.h>
#include <cstdio> // Needed to open a file in the classic way, so libcurl can write to it
#include <libxml/tree.h>   // V----------------V
#include <libxml/parser.h> // for filelist stuff

using namespace std;

void newfile(string name);
bool checkfile(string name); // true if already downloaded

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
			if(checkfile(myfilelist->getfilename(j)))
			{
				cout << "Already downloaded" << endl;
				continue;
			}
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
				newfile(myfilelist->getfilename(j));
				cout << "Done." << endl;
			}
		}
	}
	
	curl_easy_cleanup(mycurl);
	
	return 0;
}
                                                                               
void newfile(string name)
{
        // Load the current filelist:
        xmlDoc *doc;
        xmlNode *root, curr;
        string path=getenv("HOME");
        path += "/.tuxcast/files.xml";
        doc = xmlReadFile(path.c_str(), NULL, 0);
        if(doc == NULL)
        {
		doc = xmlNewDoc((xmlChar *)"1.0");
		root = xmlNewNode(NULL,(xmlChar *)"filelist");
		xmlDocSetRootElement(doc,root);
        }
	else
        	root = xmlDocGetRootElement(doc);


        // Add the new file:
        xmlNewChild(root,NULL,(xmlChar *)"file", (xmlChar *)name.c_str());
        // Save the filelist:
        xmlSaveFormatFileEnc(path.c_str(), doc, "UTF-8", 1);
}

bool checkfile(string name)
{
        // Load the filelist:
        xmlDoc *doc;
        xmlNode *root,*curr;
        string path = getenv("HOME");
        path += "/.tuxcast/files.xml";
        doc = xmlReadFile(path.c_str(),NULL,0);
        if(doc == NULL)
        {
                // No filelist, so we can be sure the file's not downloaded yet
                // A new filelist will be created when newfile() is called for this file
                return false; // Not downloaded
                // Alternately, there could be a syntax error, but since this file should be created/maintained by a (hopefully) sane program...
        }
        root = xmlDocGetRootElement(doc);
        curr = root->children;
        while(true) // is this portable...?
        {
   	if(strcmp((char*)curr->name,"file") == 0)
   	{
                 if(strcmp((char *)curr->children->content,name.c_str()) == 0)
                 {
                 	return true; // Already downloaded
                        break;
                 }
   	}
        if(curr->next == NULL)
        {
        	// No more nodes, if not already break'ed (or broken...?) then the file hasn't been downloaded
                	return false;
        }
        else
        	curr = curr->next;
        }
}
