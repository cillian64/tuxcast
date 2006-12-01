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
	if(argv[1][0] == 'a') // Add feed
		amarokdb.add_podcast("LUGRadio","http://chin.lugradio.org","/home/david/podcasts/LUGRadio");
	else if(argv[1][0] == 'd')
		amarokdb.del_podcast("LUGRadio");
	else
		cerr << "Invalid or no option specified (a | d)" << endl;
	return 0;
}

