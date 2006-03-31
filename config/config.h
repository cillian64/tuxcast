#ifndef CONFIG_H
#define CONFIG_H

#include <string>

using namespace std;

class configuration
{
	public:
		string podcastdir;
		void save(string filename);
		void load(string filename);
};

#endif
