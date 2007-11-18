#include <iostream>
#include <string>
#include <vector>
#include "rss.h"
#include "../config/config.h"
#include <libxml/tree.h>
#include <libxml/parser.h>

#include <libintl.h>
#include <locale.h>

using namespace std;

#define _(x) gettext(x)

void genxml(const vector<string> &downloads);
unsigned long count(string filesdotxml);

void clean()
{
	configuration myconfig;
	vector<string> downloads;

	myconfig.load();
	// Go over each feed in the configuration, finding each file to download
	for(configuration::feedlist::iterator feed = myconfig.feeds.begin(); feed != myconfig.feeds.end(); ++feed)
	{
		auto_ptr<filelist> files;
		printf(_("Checking (cached) feed %s.\n"), feed->name.c_str());
		try
		{
			string path=getenv("HOME");
			path+="/.tuxcast/cache/";
			path+=feed->name;

			files = parse(path);
		}
		catch( ... )
		{
			fprintf(stderr, _("Whoops, could not parse RSS feed\n"));
			fprintf(stderr, _("Continuing to next feed\n"));
			continue;
		}
		if(files.get() == NULL)
		{
			fprintf(stderr,_("Could not parse RSS feed (No exception caught)\n"));
			fprintf(stderr,_("(This should not happen)\n"));
			fprintf(stderr,_("Continuing to next feed\n"));
			continue;
		}
		for (filelist::iterator file = files->begin(); file != files->end(); ++file)
		{
			downloads.push_back(file->filename);
		}
	}
	printf(_("Generating XML...\n"));
	genxml(downloads);

	// Read the old XML file:
	unsigned long numold = count("files.xml");
	printf(ngettext("Old files.xml contains %d element\n", "Old files.xml contains %d elements\n",
				numold), numold);
	unsigned long numnew = count("files.xml.new");
	printf(ngettext("New files.xml.new contains %d element\n", "New files.xml.new contains %d elements\n",
				numnew), numnew);
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

void genxml(const vector<string> &downloads)
{
	xmlDoc *doc;
	xmlNode *root, *curr;
	string path=getenv("HOME");
	path+="/.tuxcast/files.xml.new";
	doc = xmlNewDoc((xmlChar *)"1.0");
	root = xmlNewNode(NULL,(xmlChar *)"filelist");
	xmlDocSetRootElement(doc,root);
	for(vector<string>::const_iterator download = downloads.begin();
			download != downloads.end();
			++download)
	{
		xmlNewChild(root,NULL,(xmlChar *)"file", (xmlChar *)download->c_str());
	}
	xmlSaveFormatFileEnc(path.c_str(), doc, "UTF-8", 1);
}
