#include <iostream>
#include "amarok-plugin.h"

using namespace std;

int main(int argc, char **argv)
{
	cout << "Tuxcast->amarok configuration utility" << endl;
	cout << "Version 0.01a (Tuxcast 0.2)" << endl;
	cout << endl;
	collection amarokdb;
	amarokdb.open("/home/david/.kde/share/apps/amarok/collection.db");
	if(argc == 1)
	{
		cerr << "Invalid or no option specified (a | d | e)" << endl;
		return -1;
	}
	// Could switch :/
	if(argv[1][0] == 'a') // Add feed
		amarokdb.add_podcast("LUGRadio","http://chin.lugradio.org","/home/david/podcasts/LUGRadio");
	if(argv[1][0] == 'd') // Delete feed
		amarokdb.del_podcast("LUGRadio");
	if(argv[1][0] == 'e') // Add episode
		amarokdb.add_episode("http://chin.lugradio.org/ep1.ogg","","http://chin.lugradio.org", "ChinRadio Episode 1");


	return 0;
}

