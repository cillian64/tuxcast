/*
 * 
 * This file is part of Tuxcast, "The linux podcatcher"
 * Copyright (C) 2006-2008 David Turner
 * Copyright (C) 2009, 2010 Mathew Cucuzella (kookjr@gmail.com)
 * 
 * Tuxcast is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Tuxcast is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Tuxcast; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */



#include "../compile_flags.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <stdio.h> // :)
#include "rss.h"
#include "../config/config.h"
#include <curl/curl.h>
#include <cstdio> // Needed to open a file in the classic way, so libcurl can write to it
#include <string.h>
#include <libxml/tree.h>   // V----------------V
#include <libxml/parser.h> // for filelist stuff
#include <unistd.h>
#include "tuxcast_functions.h"
#include "tuxcast_exceptions.h"
#include "../libraries/filestuff.h"
#include "../libraries/filestuff_exceptions.h"
#include "../libraries/common.h"
#include "../config/config_exceptions.h"
#include "rss_exceptions.h"
#include "tuxcast_functions.h"
#include "cleaner.h"
#include <map>
#include "../version.h"

#ifdef THREADS
#include <pthread.h>
#endif

using namespace std;


#include <libintl.h>
#include <locale.h>


const char options[] = "scuC:U:q:fvhe:";
#define _(x) gettext(x)

static bool setup_output(void);

int main(int argc, char *argv[])
{
	configuration myconfig;
	map<char, string> vars;

        // TODO: Fix this stuff?:
	setlocale(LC_ALL,"");
	bindtextdomain("tuxcast","/usr/share/locale");
	textdomain("tuxcast");
	
	
	// Remember, we can only take one primary option.
	// We store the first option, then check - if there's another, we bork out and moan.
	
	char opt1=getopt(argc,argv,options);
	string optarg1;
	if(optarg != NULL)
		optarg1=optarg;
	else
		optarg1="";
	
        // -u and -U may have second option of -e num
	char opt2=getopt(argc,argv,options);
        int episode_cnt = 1;
        if (opt2 != -1 && opt2 != 'e')
	{
		cerr << "Error, more than one option was passed and not -u or -U.  You must only pass one option." << endl;
		return -1;
	}
        else if (opt2 == 'e' && (episode_cnt = atoi(optarg)) <= 0)
        {
		cerr << "Error, invalid episode count for -e." << endl;
		return -1;
        }

	if(!((opt1 == 'h') || (opt1 == 'v'))) // Load config if option isn't h or v
	{
		try
		{
			myconfig.load();
		}
		catch(eConfig_NoConfigFile &e)
		{
			fprintf(stderr,_("Cannot load config file - please create one\n"));
			return -1; // No need to print the exception
			// We know exactly what this one means
		}
		// If theres no config file, then
		// a) checkfileexists will throw an exception, which will be
		// caught here, and we abort.
		// b) checkfileexists will return false
	}

	try
	{
                bool subprocess = false;
		switch(opt1)
		{
                        case 's':
			case 'c':
                                set_lock();
                                if (opt1 == 's') {
                                    myconfig.progress = true;
                                    subprocess = setup_output();
                                }
				printf(_("Checking all feeds\n"));
				checkall(myconfig);
				setvars(vars, myconfig);
				runhook(POSTRUN, vars, myconfig);
				xmlCleanupParser();
                                if (subprocess) {
                                    fclose(stdout);  /* tuxstatus will see EOF */
                                    wait(NULL);      /* wait for child */
                                }
				break;
			case 'u':
                                set_lock();
				printf(_("Getting up to date on all feeds\n"));
				up2dateall(myconfig, episode_cnt);
				setvars(vars, myconfig);
				runhook(POSTRUN, vars, myconfig);
				xmlCleanupParser();
				break;

			case 'C':
                                set_lock();
				printf(_("Checking feed, \"%s\"\n"), optarg1.c_str());
				// We need to loop through myconfig.feeds to find the feed ID corresponding to the passed name
				if(strcmp(optarg1.c_str(),"") == 0)
				{
					fprintf(stderr,_("You must pass a non-blank feed name\n"));
					return -1;
				}
				FOREACH(configuration::feedlist::iterator, myconfig.feeds, feed)
				{
					if(strcasecmp(optarg1.c_str(),feed->name.c_str()) == 0)
					{
						// Found the feed
						filelist files;
						check(myconfig, *feed, files);
						getlist(files, myconfig);
#ifdef THREADS
						for(int i=0; i<myconfig.threads.size(); i++)
							pthread_join(myconfig.threads[i], NULL);
#endif
						setvars(vars, myconfig);
						runhook(POSTRUN, vars, myconfig);
						xmlCleanupParser();
						return 0;
					}
				}
				// If we got all through the feeds, and it wasn't found (and we returned),
				// then the feed doesn't exist:
				fprintf(stderr,_("Unknown feed, \"%s\"\n"),optarg1.c_str());
				return -1;
				break; // Bah
			case 'U':
                                set_lock();
				printf(_("Getting up2date on feed \"%s\"\n"),optarg1.c_str());
				// We need to loop through myconfig.feeds to find the feed ID corresponding to the passed name
				if(strcmp(optarg1.c_str(),"") == 0)
				{
					fprintf(stderr,_("You must pass a non-blank feed name\n"));
					return -1;
				}
				FOREACH(configuration::feedlist::iterator, myconfig.feeds, feed)
				{
					if(strcasecmp(optarg1.c_str(),feed->name.c_str()) == 0)
					{
						// Found the feed
						filelist files;
						up2date(myconfig, *feed, files, episode_cnt);
						getlist(files, myconfig);
#ifdef THREADS
						for(int i=0; i<myconfig.threads.size(); i++)
							pthread_join(myconfig.threads[i], NULL);
#endif
						setvars(vars, myconfig);
						runhook(POSTRUN, vars, myconfig);
						xmlCleanupParser();
						return 0;
					}
				}
				// If we got all through the feeds, and it wasn't found (and we returned),
				// then the feed doesn't exist:
				fprintf(stderr,_("Unknown feed, \"%s\"\n"),optarg1.c_str());
				return -1;
				break; // Bah

			case 'q':
                                set_lock();
				cout << "Querying feed for available episodes..." << endl;

                                if (optarg1.find("http://") == 0 || optarg1.find("https://") == 0) {
                                    show_episodes(myconfig, optarg1);
                                }
                                else {
                                    if (optarg1.size() == 0) {
					fprintf(stderr,_("You must pass a non-blank feed name\n"));
					return -1;
                                    }
                                    FOREACH(configuration::feedlist::iterator, myconfig.feeds, feed) {
					if(strcasecmp(optarg1.c_str(),feed->name.c_str()) == 0) {
                                            show_episodes(myconfig, feed->address);
                                        }
                                    }
                                }
				xmlCleanupParser();
				break;
                                
			case 'f':
                                set_lock();
				cout << "Cleaning out files.xml..." << endl;
				clean();
				break;

			case 'v':
                                // no lock checking needed here, just info
				version();
				break;

			case 'h':
                                // no lock checking needed here, just info
				// Fallthrough:

			default:
                                // no lock checking needed here, just info
				printf(_("Usage: tuxcast <option>\n"));
				printf(_("where <option> is one of the below:\n"));
				printf(_("-s - Check all feeds, verbose output\n"));
				printf(_("-c - Check all feeds\n"));
				printf(_("-u [-e num] - Download only the latest num files from all feeds (default 1)\n"));
				printf(_("-C name - Download all episodes of the named feed\n"));
				printf(_("-U name [-e num] - Download only the latest episode of the named feed (default 1)\n"));
				printf(_("-q name|URL - Query feed for available episodes\n"));
				printf(_("-h - Show this help message\n"));
				printf(_("-f - clean out old podcasts from files.xml\n"));
				printf(_("-v - Show version and license information\n"));
		}
	}
	catch(eTuxcast_FSFull &e)
	{
		fprintf(stderr, _("Error: could not save config/state due to full filesystem:\n"));
		e.print();

		return -1;
	}
	catch(eProcessLock &e)
	{
		e.print();

		return -1;
	}

	return 0;
}

/*
 * If possible redirect the standard output to child process of
 * tuxstatus through a pipe. Return true if success and the
 * child process is spawned.
 */
static bool setup_output(void) {
    int pipefd[2];
    pid_t cpid;
    struct stat st;
    const char* tuxstatus_prog = "/usr/bin/tuxstatus";

    /*
     * If stdout is already a pipe, assume the user has already
     * done something line, "tuxcast -s | tuxstatus" and abort.
     */
    if (fstat(STDOUT_FILENO, &st) == -1 || (st.st_mode & S_IFIFO)) {
        return false;
    }

    /*
     * Make sure the program tuxstatus is available to exec
     */
    if (stat(tuxstatus_prog, &st) == -1) {
        return false;
    }

    /*
     * Create a pipe that will be used for tuxcast's stdout to be
     * hooked to tuxstatus' stdin.
     */
    if (pipe(pipefd) == -1) {
        return false;
    }

    /*
     * Fork the process. The child will become tuxstatus.
     */
    if ( (cpid=fork()) == -1 ) {
        return false;
    }

    if (cpid == 0) {   /* this is the child process, setup pipe as stdin */

        close(pipefd[1]);  /* close unused write end */

        /* redirect stdin to the pipe */
        FILE* tmp = fdopen(pipefd[0], "r");
        dup2(tmp->_fileno, STDIN_FILENO);
        fclose(tmp);

        /* exec the tuxstatus program */
        execl(tuxstatus_prog, "tuxstatus", "-d", (char* )NULL);

        /* only get here if exec fails */
        _exit(EXIT_SUCCESS);

    }
    else {    /* parent */

        close(pipefd[0]);   /* close unused read end */

        /* redirect stdout to pipe */
        FILE* tmp = fdopen(pipefd[1], "w");
        dup2(tmp->_fileno, STDOUT_FILENO);
        fclose(tmp);

        /* keep error messages from messing up ncurses formatted output */
        fclose(stderr);

        return true;
    }
}
