#include "../compile_flags.h"
#include <libxml/tree.h>
#include <libxml/parser.h>
#include "config.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#define fs boost::filesystem

configuration::configuration()
{
	feeds=NULL;
	numoffeeds=0;
}

configuration::~configuration()
{
	delete[] feeds;
}

void configuration::save()
{
	xmlDocPtr doc = NULL;
	xmlNodePtr root_node = NULL, node = NULL,node2=NULL;
	string path;
	fs::path mypath;
	
	doc = xmlNewDoc((xmlChar *)"1.0");
	root_node = xmlNewNode(NULL, (xmlChar *)"config");
	xmlDocSetRootElement(doc, root_node);

	xmlNewChild(root_node, NULL, (xmlChar *)"podcastdir",
			(xmlChar *)podcastdir.c_str());
	
	if(this->ask == true) // All this to convert a bool to a string...
	{
		xmlNewChild(root_node,NULL, (xmlChar *)"ask",
			(xmlChar *)"true");
	}
	else
	{
		xmlNewChild(root_node,NULL,(xmlChar *)"ask",
			(xmlChar *)"false");
	}
				
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

	path = getenv("HOME");
	path = path + "/.tuxcast";
	try
	{
		mypath = path;
	}
	catch(...)
	{
		cout << "Invalid Path in config::save()" << endl;
		return;
	}

	if(!fs::exists(mypath))
	{
		cout << "Directory ~/.tuxcast doesn't exist: creating it" << endl;
		mypath = getenv("HOME");
		mypath /= ".tuxcast";
		try
		{
			fs::create_directory(mypath);
		}
		catch(...)
		{
			cerr << "Error creating ~/.tuxcast" << endl;
			return;
		}
	}
	
	path = path + "/config.xml";
	xmlSaveFormatFileEnc(path.c_str(), doc,  "UTF-8", 1);

	xmlFreeDoc(doc);
	xmlCleanupParser();
}

void configuration::load()
{
	xmlDoc *doc;
	xmlNode *root=NULL;
	xmlNode *curr=NULL;
	string path=getenv("HOME");
	fs::path mypath;
	path = path + "/.tuxcast/config.xml";
	try
	{
		mypath = path;
	}
	catch(...)
	{
		cout << "Invalid path in config::load()" << endl;
		return;
	}
	
	if(fs::exists(mypath))
	{
		doc = xmlReadFile(path.c_str(), NULL, 0);
	if(doc == NULL)
	{
		cerr << "Error parsing your config file" << endl;
		cerr << "Please delete ~/.tuxcast/config.xml and make a new one" << endl;
		return;
	}

	else
	{
		doc = NULL;
		cout << "No config file found" << endl;
		return;
	}
		
	root = xmlDocGetRootElement(doc);
	curr = root->children;
	while(true)
		{
			if(strcmp((char *)curr->name, "podcastdir") == 0)
			{
				this->podcastdir = (char *)curr->children->content;
			}
			if(strcmp((char *)curr->name, "ask") == 0)
			{
				if(strcmp((char *)curr->children->content,"true") == 0)
				{
				this->ask = true;
			}
			else
				this->ask = false;
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
		if((strcmp((char *)curr->name, "feeds") == 0) && numoffeeds != 0)
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

