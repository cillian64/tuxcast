#include <string>
#include <sqlite3.h>

using namespace std;
// TODO: Add exceptions...

class collection
{
	public:
		collection();
		void open(string path);
		void add_podcast(string name, string url, string folder); // Add a podcast to collection->podcastchannels
		void del_podcast(string name);
		void add_episode(string url, string localurl, string parent, string title);
		
	private:
		sqlite3 *db;
};

// Callbacks:
static int callback_dud(void *cookie, int argc, char **argv, char **colname);

