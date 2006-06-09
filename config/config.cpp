#include "../compile_flags.h"
#include <libxml/tree.h>
#include <libxml/parser.h>
#include "config.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include "../libraries/filestuff.h"


configuration::configuration()
{
	// Nothing needed here anymore,
	// since the vector should initialize itself just fine
}

configuration::~configuration()
{
	// ditto
}

void configuration::save()
{
	xmlDocPtr doc = NULL;
	xmlNodePtr root_node = NULL, node = NULL,node2=NULL;
	string path;
	
	doc = xmlNewDoc((xmlChar *)"1.0");
	root_node = xmlNewNode(NULL, (xmlChar *)"config");
	xmlDocSetRootElement(doc, root_node);

	if(podcastdir[podcastdir.length()-1] == '/')
	{
		cerr << "Trimming trailing / from podcastdir..." << endl;
		podcastdir.erase(podcastdir.length()-1,1);
	}
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
				

	node=xmlNewChild(root_node,NULL, (xmlChar *)"feeds",NULL);
	for(int i=0; i<this->feeds.size(); i++)
	{
		node2=xmlNewChild(node,NULL,(xmlChar *)"feed",NULL);
		xmlNewChild(node2,NULL,(xmlChar *)"name",(xmlChar *)this->feeds[i]->name.c_str());
		xmlNewChild(node2,NULL,(xmlChar *)"address",(xmlChar *)this->feeds[i]->address.c_str());
		xmlNewChild(node2,NULL,(xmlChar *)"folder",(xmlChar *)this->feeds[i]->folder.c_str());
	}

	path = getenv("HOME");
	path = path + "/.tuxcast";
	if(!checkfolderexists(path))
	{
		cerr << "Can't create folder .tuxcast, or it exists but is not a folder" << endl;
		return;
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
	path = path + "/.tuxcast/config.xml";
	if(checkfileexists(path) == false)
	{
		cerr << "No config file found..." << endl;
		// Or BSD or boost is being used...
		return;
	}

	doc = xmlReadFile(path.c_str(), NULL, 0);
	if(doc == NULL)
	{
		cerr << "Error parsing your config file" << endl;
		cerr << "Please delete ~/.tuxcast/config.xml and make a new one" << endl;
		return;
	}
		
	root = xmlDocGetRootElement(doc);
	curr = root->children;
	while(true)
	{
		// This loops through all the main elements
		// Once an element is recognised, by an if,
		// the code "Does The Right Thing" (tm)
		if(strcmp((char *)curr->name, "podcastdir") == 0)
		{
			if(curr->children == NULL)
			{
				cerr << "Please setup a podcast directory" << endl;
				cerr << "For the moment, putting all podcasts in your home directory!" << endl;
				this->podcastdir=getenv("HOME");
			}
			else
				this->podcastdir = (char *)curr->children->content;

			if(podcastdir[podcastdir.length()-1] == '/')
			{
				cerr << "Trimming trailing / from podcastdir..." << endl;
				cerr << "If you make a custom config file, please be careful" << endl;
				podcastdir.erase(podcastdir.length()-1,1);
			}

		}
		if(strcmp((char *)curr->name, "ask") == 0)
		{
			if(strcmp((char *)curr->children->content,"true") == 0)
				this->ask = true;
			else
				this->ask = false;
		}
		
	
		
		if(strcmp((char *)curr->name, "feeds") == 0)
		{
			int i=0;
			curr = curr->children; // step into feeds
			while(true)
			{
				if(strcmp((char *)curr->name, "feed") == 0)
				{
					// We've found a feed.  Let's make a new element in our array:
					feeds.push_back(NULL);
					// Let's make a new feed in the pointer in the vector:
					feeds[feeds.size()-1] = new feed;
					curr = curr->children;
					while(true)
					{
						if((strcmp((char *)curr->name,"name") == 0) || (strcmp((char *)curr->name, "address") == 0))
						{
							// if it's either of these 2, the value CANNOT be blank!!
							if(curr->children == NULL)
							{
								cerr << "Error: blank name or address in config file, ";
								cerr << "Please delete and recreate your config file";
								cerr << " using tuxcast-config" << endl;
								return;
							}
						}

						if(strcmp((char *)curr->name, "name") == 0)
							this->feeds[i]->name = (char *)curr->children->content;
						if(strcmp((char *)curr->name, "address") == 0)
							this->feeds[i]->address = (char *)curr->children->content;
						if(strcmp((char *)curr->name, "folder") == 0)
						{
							if(curr->children == NULL)
								this->feeds[i]->folder = "";
							else
								this->feeds[i]->folder = (char *)curr->children->content;
							// Previously, if the folder was "", curr->children would be NULL,
							// so curr->children->content threw a segfault
							// Since a blank folder is perfectly valid, we must be nice about it
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

