#include "rss.h"
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <iostream>
#include <string>

using namespace std;

filelist *parse(string feed)
{
	xmlDocPtr doc;
	xmlNode *curr = NULL;
	unsigned int numoffiles = 0;
	unsigned int currfile=0;
	filelist *myfilelist;
	string URL;
	string name;
	unsigned int size;

	doc = xmlReadFile(feed.c_str(),NULL,0);

	if(doc == NULL)
		cerr << "Error reading/parsing RSS file" << endl;
	
	curr = xmlDocGetRootElement(doc);
	// curr == root node
	
	curr = curr->children;
	while(curr->type != 1) // Until we get past all the text
	{
		curr = curr->next; // Keep going
	}

	// Curr = channel

	curr = curr->children;
	while(curr != NULL) // Untill we hit the end of channel
	{
		if(curr->type == 1) // It seems trying to access the name of a text segfaults...
		{
			if(strcmp("item", (char *)curr->name) == 0)
			{
				curr = curr->children;
				while(1)
				{
					if(strcmp((char *)curr->name,"enclosure") == 0)
					{
						numoffiles++;
						break;
					}
					else
					{
						if(curr->next != NULL)
						{
							curr = curr->next;
						}
						else
						{
							break;
						}
					}
				}
				curr = curr->parent;
			}
		}

		curr = curr->next;
	}

	myfilelist = new filelist(numoffiles);

	curr = xmlDocGetRootElement(doc);
	// curr == root node
	
	curr = curr->children;
	while(curr->type != 1) // Until we get past all the text
	{
		curr = curr->next; // Keep going
	}

	// Curr = channel

	curr = curr->children;
	while(curr != NULL) // Untill we hit the end of channel
	{
		if(curr->type == 1) // It seems trying to access the name of a text segfaults...
		{
			if(strcmp("item", (char *)curr->name) == 0)
			{
				curr = curr->children;
				while(1)
				{
					if(strcmp((char *)curr->name,"enclosure") == 0)
					{
						_xmlAttr *attribute;
						
						URL = "";
						size = 0;
						
						attribute = curr->properties;
						while(true)
						{
							if(strcmp((char *)attribute->name,"url") == 0)
							{
								URL=(char *)attribute->children->content;
							}
							else
							{
								if(strcmp((char *)attribute->name,"length") == 0)
								{
									size=atoi((char *)attribute->children->content);
								}
							}
							// Do any type checking here
				
							if(attribute->next == NULL)
								break;
							attribute = attribute->next;
						}

						name = URL.substr(URL.rfind("/",URL.length())+1,URL.length()-URL.rfind("/",URL.length()));
						// Remove ?... if present...
						if(name.find("?",0) != string::npos)
						{
							name = name.substr(0,name.find("?",0));
						}
						
						myfilelist->setfile(currfile,URL,name,size);
						currfile++;
						break;
					}
					else
					{
						if(curr->next != NULL)
						{
							curr = curr->next;
						}
						else
						{
							break;
						}
					}
				}
				curr = curr->parent;
			}
		}

		curr = curr->next;
	}

	
	
	xmlFreeDoc(doc);

	return myfilelist;
}

// -------------------------------------------------------------------------
filelist::filelist(unsigned int numoffiles)
{
	files = new file[numoffiles];
	this->length = numoffiles;
	for(int i=0; i<numoffiles; i++)
	{
		setfile(i, "", "", 0);
	}
}

filelist::~filelist()
{
	delete[] files;
}

void filelist::setfile(unsigned int file, string URL, string filename, unsigned long length)
{
	files[file].URL = URL;
	files[file].filename = filename;
	files[file].length = length;
}


