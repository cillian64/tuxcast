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
#include <string>

#include <libintl.h>
#include <locale.h>

#include "../libraries/common.h"

using namespace std;

static string filenameFromUrl(const string &url);

static void parse_rss(xmlNode *rss_node, filelist &enclosed_files);
static void parse_rss_channel(xmlNode *channel_node, filelist &enclosed_files);
static void parse_rss_item(xmlNode *item_node, filelist &enclosed_files);
static void parse_rss_enclosure(xmlNode *enclosure, filelist &myfilelist);

static void parse_atom(xmlNode *feed_node, filelist &enclosed_files);
static void parse_atom_entry(xmlNode *entry_node, filelist &enclosed_files);
static void parse_atom_link(xmlNode *link_node, filelist &enclosed_files);

/* Some useful documents:
 * RDF spec: http://web.resource.org/rss/1.0/spec
 * RSS spec: http://cyber.law.harvard.edu/rss/rss.html
 * Atom spec: http://tools.ietf.org/html/rfc4287
 */

const string NS_ATOM = "http://www.w3.org/2005/Atom";
const string NS_RDF = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";

auto_ptr<filelist> parse(string feed)
{
	xmlDocPtr doc;

	doc = xmlReadFile(feed.c_str(),NULL,XML_PARSE_RECOVER | XML_PARSE_NOWARNING | XML_PARSE_NOERROR); // These options should help incorrect RSS feeds survive
	// Not ideal, but easier than convincing feed maintainers to escape stuff, etc...
	
	if(doc == NULL)
		throw eRSS_CannotParseFeed();
	
	xmlNode *root = xmlDocGetRootElement(doc);
	if(root == NULL)
		throw eRSS_CannotParseFeed();

	auto_ptr<filelist> enclosed_files(new filelist());

	try
	{
		const char *root_name = (const char *)root->name;
		if(strcasecmp(root_name,"rss") == 0)
			parse_rss(root, *enclosed_files);
		else if(strcasecmp(root_name,"RDF") == 0)
			// For the moment, RSS and RDF can be treated the same after identification
			// (RDF is in a namespace, unlike RSS, but for now we're ignoring namespaces)
			parse_rss(root, *enclosed_files);
		else if(strcasecmp(root_name,"feed") == 0)
			// TODO: write parse_atom
			parse_atom(root, *enclosed_files);
		else
			// We don't know what this document is...
			throw eRSS_InvalidRootNode();
	}
	catch (...)
	{
		xmlFreeDoc(doc);
		throw;
	}

	xmlFreeDoc(doc);

	return enclosed_files;
}

void parse_rss(xmlNode *rss_node, filelist &enclosed_files)
{
	FOREACH_XMLCHILD(rss_node, rss_part)
	{
		if (rss_part->type != XML_ELEMENT_NODE)
			continue;

		// An <rss> or <RDF> node can contain <item> and/or <channel> nodes
		if (strcasecmp("item", (char *)rss_part->name) == 0)
			parse_rss_item(rss_part, enclosed_files);
		else if (strcasecmp("channel", (char *)rss_part->name) == 0)
			parse_rss_channel(rss_part, enclosed_files);
	}
}

void parse_rss_channel(xmlNode *channel_node, filelist &enclosed_files)
{
	FOREACH_XMLCHILD(channel_node, channel_part)
	{
		if (channel_part->type != XML_ELEMENT_NODE)
			continue;

		if (strcasecmp("item", (char *)channel_part->name) == 0)
			parse_rss_item(channel_part, enclosed_files);
	}
}

void parse_rss_item(xmlNode *item_node, filelist &enclosed_files)
{
	FOREACH_XMLCHILD(item_node, item_part)
	{
		if (item_part->type != XML_ELEMENT_NODE)
			continue;

		if (strcasecmp("enclosure", (char *)item_part->name) == 0)
			parse_rss_enclosure(item_part, enclosed_files);
	}
}

static void parse_rss_enclosure(xmlNode *enclosure, filelist &enclosed_files)
{
	string URL="";
	off_t size=0;

	FOREACH_XMLATTR(enclosure, attribute)
	{
		if(strcasecmp((char *)attribute->name,"url") == 0)
		{
			URL=(char *)attribute->children->content;
		}
		else if(strcasecmp((char *)attribute->name,"length") == 0)
		{
			size=strtoul((char *)attribute->children->content, NULL, 10);
		}
		// Do any type checking here
	}

	file new_file;
	// Setting up the file we made earlier...:
	new_file.filename = filenameFromUrl(URL);
	new_file.URL = URL; // Watch out - filename != name && length != size
	new_file.length = size;

	enclosed_files.push_back(new_file);
}

static void parse_atom(xmlNode *feed_node, filelist &enclosed_files)
{
	FOREACH_XMLCHILD(feed_node, feed_part)
	{
		if (feed_part->type != XML_ELEMENT_NODE)
			continue;

		// An <atom:feed> can contain <atom:entry> nodes
		if (strcasecmp("entry", (char *)feed_part->name) == 0)
			parse_atom_entry(feed_part, enclosed_files);
	}
}

static void parse_atom_entry(xmlNode *entry_node, filelist &enclosed_files)
{
	FOREACH_XMLCHILD(entry_node, entry_part)
	{
		if (entry_part->type != XML_ELEMENT_NODE)
		{
			continue;
		}

		// An <atom:entry> can contain <atom:link> nodes
		if (strcasecmp("link", (char *)entry_part->name) == 0)
		{
			parse_atom_entry(entry_part, enclosed_files);
		}
	}
}

static void parse_atom_link(xmlNode *link_node, filelist &enclosed_files)
{
	string href;
	string rel;
	off_t size=0;

	FOREACH_XMLATTR(link_node, attribute)
	{
		if (strcasecmp((char *)attribute->name,"href") == 0)
			href = (char *)attribute->children->content;
		else if (strcasecmp((char *)attribute->name,"rel") == 0)
			rel = (char *)attribute->children->content;
		else if (strcasecmp((char *)attribute->name,"length") == 0)
			size = strtoul((char *)attribute->children->content, NULL, 10);
		// Do any type checking here
	}

	if (rel == "enclosure")
	{
		// TODO - this might not be a podcast. The type attribute would be useful
		//  in guessing this with greater reliability
		enclosed_files.push_back(file(filenameFromUrl(href), href, size));
	}
}

static string filenameFromUrl(const string &url)
{
	string name = url.substr(
			url.rfind("/", url.length())+1,
			url.length()-url.rfind("/",url.length()));

	// Remove ?... if present...
	// XXX: if the URL contains: ".../?..." then name = "". That would be a bad thing for a filename
	if (name.find("?",0) != string::npos)
	{
		name = name.substr(0,name.find("?",0));
	}

	return name;
}
