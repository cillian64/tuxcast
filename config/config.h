#ifndef CONFIG_H
#define CONFIG_H

#include <string>

using namespace std;

class feed;

class configuration
{
	public:
		configuration();
		~configuration();
		
		string podcastdir;
		bool ask; // Though a string is stored in config
			// it's converted to a bool on load/save
		feed *feeds;
		unsigned int numoffeeds;
		void save();
		void load();
};

class feed
{
	public:
		string name;
		string address;
		string folder;
};

#endif
