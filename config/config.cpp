#include <libxml/tree.h>
#include <libxml/parser.h>
#include "config.h"
#include <iostream>
#include <sstream>

configuration::configuration()
{
	feeds=NULL;
	numoffeeds=0;
}

configuration::~configuration()
{
	delete[] feeds;
}

void configuration::save(string filename)
{
	xmlDocPtr doc = NULL;
	xmlNodePtr root_node = NULL, node = NULL,node2=NULL;

	doc = xmlNewDoc((xmlChar *)"1.0");
	root_node = xmlNewNode(NULL, (xmlChar *)"config");
	xmlDocSetRootElement(doc, root_node);

	xmlNewChild(root_node, NULL, (xmlChar *)"podcastdir",
			(xmlChar *)podcastdir.c_str());
	std::ostringstream o;  o << this->numoffeeds;		// !?!?!?!?!?
	xmlNewChild(root_node, NULL, (xmlChar *)"numoffeeds",
			(xmlChar *)o.str().c_str());

	node=xmlNewChild(root_node,NULL, (xmlChar *)"feeds",NULL);
	for(int i=0; i<this->numoffeeds; i++)
	{
		node2=xmlNewChild(node,NULL,(xmlChar *)"feed",NULL);
		xmlNewChild(node2,NULL,(xmlChar *)"name",(xmlChar *)this->feeds[i].name.c_str());
		xmlNewChild(node2,NULL,(xmlChar *)"address",(xmlChar *)this->feeds[i].address.c_str());
	}

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
	while(true)
	{
		if(strcmp((char *)curr->name, "podcastdir") == 0)
		{
			this->podcastdir = (char *)curr->children->content;
		}
		if(strcmp((char *)curr->name, "numoffeeds") == 0)
		{
			this->numoffeeds=atoi((char *)curr->children->content);
		}
		if((this->feeds == NULL) && (numoffeeds != 0))
		{
			this->feeds = new feed[numoffeeds];
		}
		// WARNING: At the moment, an evil config file could buffer overflow by having more <feed>s than specified in <numoffeeds>
		if(strcmp((char *)curr->name, "feeds") == 0)
		{
			int i=0;
			curr = curr->children; // step into feeds
			while(true)
			{
					if(strcmp((char *)curr->name, "feed") == 0)
				{
					curr = curr->children;
					while(true)
					{
						if(strcmp((char *)curr->name, "name") == 0)
						{
							this->feeds[i].name = (char *)curr->children->content;
						}
						if(strcmp((char *)curr->name, "address") == 0)
						{
							this->feeds[i].address = (char *)curr->children->content;
						}

						if(curr->next != NULL)
							curr = curr->next;
						else
							break;
					}
					i++;
					curr = curr->parent;
				}
				if(curr->next == NULL)
					break;
				else
					curr = curr->next;
			}
			curr = curr->parent;
		}

		if(curr->next == NULL)
		{
			break;
		}
		curr = curr->next;
	}
}

