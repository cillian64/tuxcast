#include "backend.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	backend *mybackend;
	char tmp;
	int tmp2;
	string tmp3;
	
	if(argc == 3)
	{
		// Assume they passed a server and port:
		mybackend = new backend(argv[1], atoi(argv[2]));
	}
	else
		if(argc == 2)
			// Assume just server passed:
			mybackend = new backend(argv[1]);
		else
			// Just do default
			mybackend = new backend();

	// Connect:
	try
	{
		mybackend->connect();
	}
	catch(eException &e)
	{
		cerr << "Cannot connect:" << endl;
		e.print();
		return -1;
	}

	cout << "Connected! :D" << endl;

	cout << "Enter c for check, or u for update: ";
	cin >> tmp;
	cout << "Enter feed name: ";
	cin >> tmp3;
	switch(tmp)
	{
		case 'c':
			mybackend->check(tmp3);
			break;

		case 'u':
			mybackend->up2date(tmp3);
			break;

		default:
			cerr << "Invalid option" << endl;
			return -1;
	}
			


	// Disconnect:
	mybackend->disconnect();
	
	return 0;
}

