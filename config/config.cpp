/*
 * 
 * This file is part of Tuxcast, "The linux podcatcher"
 * Copyright (C) 2006-2008 David Turner
 * 
 * Tuxcast is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Tuxcast is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Tuxcast; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */



#include "../compile_flags.h"
#include <libxml/tree.h>
#include <libxml/parser.h>
#include "config.h"
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include "../libraries/filestuff.h"
#include "../libraries/filestuff_exceptions.h"
#include "../libraries/common.h"
#include "config_exceptions.h"
#include <libintl.h>
#include <locale.h>

#define _(x) gettext(x)

using namespace std;

void configuration::save()
{
	xmlDocPtr doc = NULL;
	xmlNodePtr root_node = NULL, node = NULL,node2=NULL;
	string path;
	string cachepath;
	
	doc = xmlNewDoc((xmlChar *)"1.0");
	root_node = xmlNewNode(NULL, (xmlChar *)"config");
	xmlDocSetRootElement(doc, root_node);

	if(podcastdir[podcastdir.length()-1] == '/')
	{
		fprintf(stderr,_("Trimming trailing / from podcastdir...\n"));
		podcastdir.erase(podcastdir.length()-1,1);
	}
	xmlNewChild(root_node, NULL, (xmlChar *)"podcastdir",
			(xmlChar *)podcastdir.c_str());
#ifdef THREADS
	char temp[4];
	snprintf(temp,3,"%d",this->numofdownloaders);

	xmlNewChild(root_node, NULL, (xmlChar *)"numofdownloaders",
			(xmlChar *)temp);
#endif
			
	
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
	FOREACH(feedlist::const_iterator, feeds, feed)
	{
		node2=xmlNewChild(node,NULL,(xmlChar *)"feed",NULL);
		xmlNewChild(node2,NULL,(xmlChar *)"name",(xmlChar *)feed->name.c_str());
		xmlNewChild(node2,NULL,(xmlChar *)"address",(xmlChar *)feed->address.c_str());
		xmlNewChild(node2,NULL,(xmlChar *)"folder",(xmlChar *)feed->folder.c_str());
	}

	node=xmlNewChild(root_node,NULL, (xmlChar *)"permittedmimetypes", NULL);
	FOREACH(mimelist::const_iterator, permitted_mimes, mime)
	{
		node2=xmlNewChild(node,NULL,(xmlChar *)"mimetype",(xmlChar*)mime->c_str());
	}

	xmlNewChild(root_node,NULL, (xmlChar *)"postdownload", (xmlChar*)postdownload.c_str());
	xmlNewChild(root_node,NULL, (xmlChar *)"postrun", (xmlChar*)postrun.c_str());
	xmlNewChild(root_node,NULL, (xmlChar *)"incorrectmime", (xmlChar*)incorrectmime.c_str());



	path = getenv("HOME");
	path = path + "/.tuxcast";
	cachepath = path + "/cache";
	try
	{
		checkfolderexists(path); // If an exception is thrown, the stack should
		checkfolderexists(cachepath);
	// Be unrolled all the way back to the beginning, where it'll display and quit.
		checkfileexists(path+"/config.xml");
	}
	catch(eFilestuff_CannotCreateFolder &e)
	{
		fprintf(stderr,_("Ooops, couldn't save your new config:\n"));
		fprintf(stderr,_("Exception caught: "));
		e.print();
		throw eConfig_CannotSaveConfig();
	}
	catch(eFilestuff_NotAFile &e)
	{
		fprintf(stderr,_("Opps, couldn't save your new config,\n"));
		fprintf(stderr,_("~/.tuxcast/config.xml exists but isn't a file\n"));
		fprintf(stderr,_("Exception caught: "));
		e.print();
		throw eConfig_CannotSaveConfig();
	}
		
	path = path + "/config.xml";
	char *tempsave;
	tempsave = new char[path.size()+7];
	strcpy(tempsave,path.c_str());
	strcat(tempsave,".XXXXXX");
	mkstemp(tempsave);

	if(xmlSaveFormatFileEnc(tempsave, doc,  "UTF-8", 1) < 0)
	{
		fprintf(stderr,_("Failed to save configuration\n"));
		delete[] tempsave;
		throw eConfig_CannotSaveConfig();
	} // Nice thing is, if the FS is full, this fails and the old config
	// is left intact, instead of being truncated.

	if(!move(tempsave,path))
	{
		fprintf(stderr,_("Failed to update configuration file\n"));
		delete[] tempsave;
		throw eConfig_CannotSaveConfig();
	}
	delete[] tempsave;
	xmlFreeDoc(doc);
	xmlCleanupParser();
}

void configuration::load()
{
	xmlDoc *doc;
	xmlNode *root=NULL;
	xmlNode *curr=NULL;
	string path=getenv("HOME");
	bool queuesave=false; // This is so something in here can queue a save
	// which will take place once the XML file is closed.
	path = path + "/.tuxcast/config.xml";
	try
	{
		if(checkfileexists(path) == false)
		{
			fprintf(stderr,_("No config file found...\n"));
			throw eConfig_NoConfigFile();
		}
	}
	catch(eFilestuff_NotAFile &e)
	{
		fprintf(stderr,_("No config file found: Aborting\n"));
		throw eConfig_NoConfigFile();
	}
	// The caller should catch eNoConfigFile
	// If configuration is essential, the program should abort
	// If config is optional, it can catch and continue
	doc = xmlReadFile(path.c_str(), NULL, 0);
	if(doc == NULL)
	{
		fprintf(stderr,_("Error parsing your config file\n"));
		fprintf(stderr,_("Please delete ~/.tuxcast/config.xml and make a new one\n"));
		throw eConfig_NoConfigFile(); // Yeh, I know this isn't really
		// the right thing to call, but it works just fine
		// Nothing should need to know the difference between
		// An invalid config or none at all
	}
		
	
	root = xmlDocGetRootElement(doc);
	FOREACH_XMLCHILD(root, curr)
	{
		if (curr->type != XML_ELEMENT_NODE)
			continue;
		// This loops through all the main elements
		// Once an element is recognised, by an if,
		// the code "Does The Right Thing" (tm)
		if(strcasecmp((char *)curr->name, "podcastdir") == 0)
		{
			if(curr->children == NULL)
			{
				fprintf(stderr,_("You have not setup a podcast directory - tuxcast will use your home directory, ~.\n"));
				this->podcastdir=getenv("HOME");
				queuesave=true;
			}
			else
				this->podcastdir = (char *)curr->children->content;

			if(podcastdir[podcastdir.length()-1] == '/')
			{
				fprintf(stderr,_("Trimming trailing / from podcastdir...\n"));
				fprintf(stderr,_("If you make a custom config file, please be careful\n"));
				podcastdir.erase(podcastdir.length()-1,1);
			}

		}
		else if(strcasecmp((char *)curr->name, "ask") == 0)
		{
			if(strcasecmp((char *)curr->children->content,"true") == 0)
				this->ask = true;
			else
				this->ask = false;
		}
#ifdef THREADS
		else if(strcasecmp((char *)curr->name, "numofdownloaders") == 0)
			this->numofdownloaders = atoi((char*)curr->children->content);
#endif
		else if(strcasecmp((char *)curr->name, "permittedmimetypes") == 0)
		{
			FOREACH_XMLCHILD(curr, mimenode)
			{
				// We're only interested in elements
				if (mimenode->type != XML_ELEMENT_NODE)
					continue;
				if(mimenode->children == NULL)
				{
					fprintf(stderr,_("Warning, empty mime type in config file\n"));
					continue;
				}
				if(mimenode->children->type != XML_TEXT_NODE)
				{
					fprintf(stderr,_("Warning, mime mode with non-textual content in config file\n"));
					continue;
				}
				this->permitted_mimes.push_back((char *)mimenode->children->content);
			}
		}
		else if((strcasecmp((char *)curr->name, "feeds") == 0))
		{
			// step into feeds
			FOREACH_XMLCHILD(curr, feednode)
			{
				if(feednode->type != XML_ELEMENT_NODE)
					continue;
				if (strcasecmp((char *)feednode->name, "feed") == 0)
				{
					// We've found a feed.
					feed new_feed;
					FOREACH_XMLCHILD(feednode, partnode)
					{
						if(partnode->type != XML_ELEMENT_NODE)
							continue;
						if((strcasecmp((char *)partnode->name,"name") == 0) || (strcasecmp((char *)partnode->name, "address") == 0))
						{
							// if it's either of these 2, the value CANNOT be blank!!
							if(partnode->children == NULL)
							{
								fprintf(stderr,_("Error: blank name or address in config file, please delete and recreate your config file using tuxcast-config\n"));
								return;
							}
						}

						if(strcasecmp((char *)partnode->name, "name") == 0)
							new_feed.name = (char *)partnode->children->content;
						else if(strcasecmp((char *)partnode->name, "address") == 0)
							new_feed.address = (char *)partnode->children->content;
						else if(strcasecmp((char *)partnode->name, "folder") == 0)
						{
							if(partnode->children == NULL)
								new_feed.folder = "";
							else
								new_feed.folder = (char *)partnode->children->content;
							// Previously, if the folder was "", partnode->children would be NULL,
							// so feednode->children->content threw a segfault
							// Since a blank folder is perfectly valid, we must be nice about it
						}
					}

					feeds.push_back(new_feed);
				}
			}
		}
		else if((strcasecmp((char *)curr->name, "postdownload") == 0))
		{
			if(curr->children != NULL)
				postdownload = (char *)curr->children->content;
		}
		
		else if((strcasecmp((char *)curr->name, "postrun") == 0))
		{
			if(curr->children != NULL)
				postrun = (char *)curr->children->content;
		}
		else if((strcasecmp((char *)curr->name, "incorrectmime") == 0))
		{
			if(curr->children != NULL)
				incorrectmime = (char *)curr->children->content;
		}

	}

	if(queuesave == true)
		this->save();


}

void feed::displayConfig(void) const
{
	printf(_("Name: %s\n"),name.c_str());
	printf(_("Address: %s\n"),address.c_str());
	printf(_("Folder: %s\n"),folder.c_str());
}
