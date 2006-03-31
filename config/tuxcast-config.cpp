#include <iostream>
#include "config.h"

using namespace std;

int main()
{
	configuration myconfig;
	
	cout << "Tuxcast Config" << endl;

	myconfig.load("config.xml");

	return 0;
}

