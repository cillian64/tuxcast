#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>

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
		vector<feed *> feeds; // A vector of feed pointers
		
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
