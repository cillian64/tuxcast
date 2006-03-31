#include <libxml/tree.h>
#include <libxml/parser.h>
#include "config.h"
#include <iostream>

void configuration::save(string filename)
{
	xmlDocPtr doc = NULL;
	xmlNodePtr root_node = NULL, node = NULL;

	doc = xmlNewDoc((xmlChar *)"1.0");
	root_node = xmlNewNode(NULL, (xmlChar *)"config");
	xmlDocSetRootElement(doc, root_node);

	xmlNewChild(root_node, NULL, (xmlChar *)"podcastdir",
			(xmlChar *)podcastdir.c_str());

	xmlSaveFormatFileEnc(filename.c_str(), doc,  "UTF-8", 1);

	xmlFreeDoc(doc);
	xmlCleanupParser();
}

void configuration::load(string filename)
{
	xmlDoc *doc;
	xmlNode *root=NULL;
	xmlNode *curr=NULL;

	doc = xmlReadFile(filename.c_str(), NULL, 0);
	if(doc == NULL)
	{
		cerr << "Error loading config file" << endl;
	}
	root = xmlDocGetRootElement(doc);
	curr = root->children;
	while(strcmp((char *)curr->name,"podcastdir") != 0)
	{
		cout << "Skipping node named " << curr->name << endl;
		if(curr->next == NULL)
		{
			cout << "Done" << endl;
			break;
		}
		curr = curr->next;
	}
	cout << "Node name is " << curr->name << endl;
	cout << "Podcastdir is: " << curr->children->content << endl;
}

