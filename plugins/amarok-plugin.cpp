#include <sqlite3.h>
#include "amarok-plugin.h"

collection::collection()
{
	db = 0; // So we can see if the DB is uninitialized.
}

void collection::open(string path)
{ // Partially stolen from the sqlite3 quick tutorial :P
	int returncode;

	returncode=sqlite3_open(path.c_str(), &db);
	if(returncode) // != 0
	{
		// throw someexception()
		db=0;
	}
	
}

void collection::add_podcast(string name, string url, string folder)
{
	int returncode;
	char *errormsg;
	string sql="INSERT INTO podcastchannels (url,title,directory,autoscan,fetchtype,autotransfer,haspurge) VALUES ('"+url+"','"+name+"','"+folder+"',0,0,1,0);";
	if(db == 0)
	{
		// throw someexception()
		return;
	}

	returncode = sqlite3_exec(db, sql.c_str(), callback_dud, 0, &errormsg);
	if(returncode != SQLITE_OK)
	{
		// throw someexception()
		return;
	}
}

void collection::del_podcast(string name)
// Look familiar?:
{
	int returncode;
	char *errormsg;
	string sql="DELETE FROM podcastchannels WHERE title == '"+name+"';";
	if(db == 0)
	{
		// throw someexception()
		return;
	}

	returncode = sqlite3_exec(db, sql.c_str(), callback_dud, 0,  &errormsg);
	if(returncode != SQLITE_OK)
	{
		// throw someexception()
		return;
	}
}

static int callback_dud(void *cookie, int argc, char **argv, char **colname)
// Callback for sqlite, which does BuggerAll (tm)
{
	return 0;
}

void collection::add_episode(string url, string localurl, string parent, string title)
{ // *cough* *cough*
	int returncode;
	char *errormsg;
	string sql="INSERT INTO podcastepisodes (url,localurl,parent,title,isNew) VALUES ('"+url+"','"+localurl+"','"+parent+"','"+title+"',1);";
	if(db == 0)
	{
		// throw someexception()
		return;
	}

	returncode = sqlite3_exec(db, sql.c_str(), callback_dud, 0, &errormsg);
	if(returncode != SQLITE_OK)
	{
		// throw someexception()
		return;
	}
}
