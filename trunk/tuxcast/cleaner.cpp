#include <iostream>
#include <string>
#include <vector>
#include "rss.h"
#include "../config/config.h"

using namespace std;

void genxml(vector<string*> *downloads);
unsigned long count(string filesdotxml);

void clean()
{
	configuration *myconfig = new configuration;
	vector<string*> *downloads;

	myconfig->load();
	downloads = new vector<string*>;
	for(unsigned int i=0; i<myconfig->feeds.size(); i++)
	{
		filelist *files;
		cout << "Checking feed \"" << myconfig->feeds[i]->name << "\"." << endl;
		try
		{
			string path=getenv("HOME");
			path+="/.tuxcast/cache/";
			path+=myconfig->feeds[i]->name;

			files = parse(path);
		}
		catch( ... )
		{
			cerr << "Whoops, could not parse RSS feed" << endl;
			cerr << "Continuing to next feed" << endl;
			continue;
		}
		if(files == NULL)
		{
			cerr << "Could not parse RSS feed (No exception caught...?)" << endl;
			cerr << "This shouldn't happen" << endl;
			cerr << "Continuing to next feed" << endl;
			continue;
		}
		for(unsigned int j=0; j<files->size(); j++)
		{
			string *newfile = new string;
			*newfile = (*files)[j]->filename;
			downloads->push_back(newfile);
		} // TODO: Can these for loops be merged?
		for(unsigned int j=0; j<files->size(); j++)
			delete (*files)[j];
		delete files;
	}
	cout << "Generating XML..." << endl;
	genxml(downloads);

	// Read the old XML file:
	cout << "Old files.xml contains " << count("files.xml") << " elements" << endl;
	cout << "New files.xml.new contains " << count("files.xml.new") << " elements" << endl;
	cout << "(I won't replace your files.xml as this feature is still immature.  If you're feeling brave, replace ~/.tuxcast/files.xml with ~/.tuxcast/files.xml.new)" << endl;;

}

unsigned long count(string filesdotxml)
{
	xmlDoc *doc;
	xmlNode *root, *curr;
	unsigned long counter=0;
	string path=getenv("HOME");
	path+="/.tuxcast/";
	path+=filesdotxml;
	curr=NULL;
	doc = xmlReadFile(path.c_str(), NULL, XML_PARSE_RECOVER);
	if(doc == NULL)
		return 0;
	
	root = xmlDocGetRootElement(doc);
	if(root == NULL)
		return 0;
	
	if(strcasecmp((char*)root->name,"filelist") != 0)
		return 0;
	curr = root->children;
	while(true)
	{
		if(strcasecmp((char *)curr->name,"file") == 0)
			counter++;
		if(curr->next != NULL)
			curr = curr->next;
		else
			return counter;
	}
}

void genxml(vector<string*> *downloads)
{
	xmlDoc *doc;
	xmlNode *root, *curr;
	string path=getenv("HOME");
	path+="/.tuxcast/files.xml.new";
	doc = xmlNewDoc((xmlChar *)"1.0");
	root = xmlNewNode(NULL,(xmlChar *)"filelist");
	xmlDocSetRootElement(doc,root);
	for(unsigned int i=0; i<downloads->size(); i++)
		xmlNewChild(root,NULL,(xmlChar *)"file", (xmlChar *)((*downloads)[i])->c_str());
	xmlSaveFormatFileEnc(path.c_str(), doc, "UTF-8", 1);
}
