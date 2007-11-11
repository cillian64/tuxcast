/*
 * 
 * This file is part of Tuxcast, "The linux podcatcher"
 * Copyright (C) 2006-2007 David Turner
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

#define _(x) gettext(x)

#include "../compile_flags.h"
#include "rss.h"
#include "rss_exceptions.h"
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <iostream>
#include <string>

#include <libintl.h>
#include <locale.h>

using namespace std;

static void addtolist(filelist &myfilelist, xmlNode *enclosure);

auto_ptr<filelist> parse(string feed)
{
	xmlDocPtr doc;
	xmlNode *curr = NULL;
	bool rss2; // true == 2, false == 1 or RDF.


	doc = xmlReadFile(feed.c_str(),NULL,XML_PARSE_RECOVER | XML_PARSE_NOWARNING | XML_PARSE_NOERROR); // These options should help incorrect RSS feeds survive
	// Not ideal, but easier than convincing feed maintainers to escape stuff, etc...
	
	if(doc == NULL)
		throw eRSS_CannotParseFeed();
	
	curr = xmlDocGetRootElement(doc);
	// curr == root node
	if(curr == NULL)
		throw eRSS_CannotParseFeed();
	
	if(strcasecmp((char*)curr->name,"rss") == 0)
		rss2 = true;
	else
		if(strcasecmp((char*)curr->name,"RDF") == 0)
			rss2 = false;
		else
			throw eRSS_InvalidRootNode();
	
	curr = curr->children;
	while(curr->type != 1) // Until we get past all the text
		curr = curr->next; // Keep going

	// All that mucking about counting stuff is eliminated, thanks to STL :-)

	auto_ptr<filelist> myfilelist(new filelist());

	curr = xmlDocGetRootElement(doc);
	// curr == root node
	
	curr = curr->children;
	while(curr->type != 1) // Until we get past all the text
		curr = curr->next; // Keep going

	// Curr = channel

	curr = curr->children;
	if(rss2 == true) // RSS 2.0 mode:
	{
		while(true) // Untill we hit the end of channel
		{
			if(curr->type == XML_ELEMENT_NODE) // It seems trying to access the name of a text segfaults...
				if(strcasecmp("item", (char *)curr->name) == 0)
				{
					curr = curr->children;
					while(1)
						if(strcasecmp((char *)curr->name,"enclosure") == 0)
						{
							addtolist(*myfilelist, curr);
							break;
						}
						else
							if(curr->next != NULL)
								curr = curr->next;
							else
								break;
					curr = curr->parent;
				}
			if(curr->next == NULL)
				break;
			else
				curr = curr->next;
		}
	}
	else
	{ // RSS 1.0 mode:
		curr = curr->parent; // Skip out of channel.
		while(true)
		{
			if(curr->type == XML_ELEMENT_NODE)
				if(strcasecmp((char*)curr->name, "item") == 0)
				{
					curr = curr->children; // Step inside item
					{
						// This next loop means keep going till you get to enclosure.
						while(curr->type != 1) // It has to be a bit complicated to avoid a segfault:
						// Non-item nodes have a NULL ->name I think.
						{
							curr = curr->next;
							if((curr->type == 1) && (strcasecmp((char*)curr->name,"enclosure") != 0))
								curr = curr->next;
						}
						// Curr now == the enclosure:
						addtolist(*myfilelist,curr);
					}
					curr = curr->parent; // Step out
				}
			if(curr->next == 0)
				break;
			else
				curr = curr->next;
		}
	}

	xmlFreeDoc(doc);

	return myfilelist;
}

static void addtolist(filelist &myfilelist, xmlNode *enclosure)
{
	string name=""; // Is worked out from trimming URL
	string URL="";
	unsigned int size=0;
	
	for (_xmlAttr *attribute = enclosure->properties;
			attribute != NULL; attribute = attribute->next)
	{
		if(strcasecmp((char *)attribute->name,"url") == 0)
		{
			URL=(char *)attribute->children->content;
		}
		else if(strcasecmp((char *)attribute->name,"length") == 0)
		{
			size=atoi((char *)attribute->children->content);
		}
		// Do any type checking here
	}

	file new_file;

	name = URL.substr(URL.rfind("/",URL.length())+1,URL.length()-URL.rfind("/",URL.length()));
	// Remove ?... if present...
	// XXX: if the URL contains: ".../?..." then name = "". That would be a bad thing for a filename
	if(name.find("?",0) != string::npos)
	{
		name = name.substr(0,name.find("?",0));
	}
	// Setting up the file we made earlier...:
	new_file.filename = name;
	new_file.URL = URL; // Watch out - filename != name && length != size
	new_file.length = size;

	myfilelist.push_back(new_file);
}
