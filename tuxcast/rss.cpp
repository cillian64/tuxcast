#include "../compile_flags.h"
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
	filelist *myfilelist;
	string URL;
	string name;
	unsigned int size; // the length of the file, in bytes I think

	doc = xmlReadFile(feed.c_str(),NULL,XML_PARSE_RECOVER | XML_PARSE_NOWARNING | XML_PARSE_NOERROR); // These options should help incorrect RSS feeds survive
	// Not ideal, but easier than convincing feed maintainers to escape stuff, etc...

	if(doc == NULL)
	{
		cerr << "Error reading/parsing RSS file" << endl;
		return NULL;
	}
	
	curr = xmlDocGetRootElement(doc);
	// curr == root node
	
	curr = curr->children;
	while(curr->type != 1) // Until we get past all the text
	{
		curr = curr->next; // Keep going
	}


	// All that mucking about counting stuff is eliminated, thanks to STL :-)

	myfilelist = new filelist();

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
						// FILE FOUND!!! :-)
						// Let's add a new element to the vector:
						myfilelist->files.push_back(NULL);
						myfilelist->files[myfilelist->files.size()-1] = new file;
						
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
						// Setting up the file we made earlier...:
						myfilelist->files[myfilelist->files.size()-1]->filename = name;
						myfilelist->files[myfilelist->files.size()-1]->URL = URL; // Watch out - filename != name && length != size
						myfilelist->files[myfilelist->files.size()-1]->length = size;


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

