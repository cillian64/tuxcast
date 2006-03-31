#include <iostream>
#include "rss.h"
#include "../config/config.h"

using namespace std;

int main(void)
{
	filelist *myfilelist;
	
	cout << "Hello, World" << endl;

	myfilelist = parse("http://curry.podshow.com/?feed=rss2"); // parse takes a filelist pointer, makes a new filelist, and fills it

	for(int i=0; i<myfilelist->numoffiles(); i++)
	{
		cout << "File found..." << endl;
		cout << "name is " << myfilelist->getfilename(i) << endl;
		cout << "length is " << myfilelist->getlength(i) << endl;
		cout << "URL is " << myfilelist->getURL(i) << endl;
	}

	
	return 0;
}

