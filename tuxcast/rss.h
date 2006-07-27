#include <string>
#include <iostream>
#include <vector>

using namespace std;

class file
{
	public:
		string filename;
		string URL;
		unsigned long length;
};

class filelist
{
	public:

		// Just opening these up to the public eliminates all that
		// messing about with accessor functions, etc....
		vector<file *> files; // Note - it's a vector of FILE POINTERS!!!
		// This means you'll have to access it like files[123]->...
		// instead of files[123].xyz
		
		// Length is replaced by files.size()
};

filelist *parse(string feed);

