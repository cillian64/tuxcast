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
		feed *feeds;
		unsigned int numoffeeds;
		void save(string filename);
		void load(string filename);
};

class feed
{
	public:
		string name;
		string address;
};

#endif
