#include <string>
#include <iostream>

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
		filelist(unsigned int numoffiles);
		~filelist(void);

		void setfile(unsigned int file, string URL, string filename, unsigned long length);
		string getURL(unsigned int file)
		{ return files[file].URL; }
		string getfilename(unsigned int file)
		{ return files[file].filename; }
		unsigned long getlength(unsigned int file)
		{ return files[file].length; }

		unsigned int numoffiles(void)
		{ return this->length; }

	private:
		
		file *files;
		unsigned int length;
};

filelist *parse(string feed);

