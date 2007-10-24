#include <iostream>
#include <string>
#include <vector>
#include "rss.h"
#include "../config/config.h"

#include <libintl.h>
#include <locale.h>

using namespace std;

#define _(x) gettext(x)

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
		printf(_("Checking (cached) feed %s."), myconfig->feeds[i]->name.c_str());
		try
		{
			string path=getenv("HOME");
			path+="/.tuxcast/cache/";
			path+=myconfig->feeds[i]->name;

			files = parse(path);
		}
		catch( ... )
		{
			fprintf(stderr, _("Whoops, could not parse RSS feed\n"));
			fprintf(stderr, _("Continuing to next feed\n"));
			continue;
		}
		if(files == NULL)
		{
			fprintf(stderr,_("Could not parse RSS feed (No exception caught)\n"));
			fprintf(stderr,_("(This should not happen)\n"));
			fprintf(stderr,_("Continuing to next feed\n"));
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
	printf(_("Generating XML...\n"));
	genxml(downloads);

	// Read the old XML file:
	printf(_("Old files.xml contains %d elements\n"), count("files.xml"));
	printf(_("New files.xml.new contains %d elements\n"), count("files.xml.new"));
	printf(_("(I won't replace your files.xml as this feature is still immature.\n"));
	printf(_("If you're feeling brave, replace ~/.tuxcast/files.xml with ~/.tuxcast/files.xml.new)\n"));

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
