/*
 * 
 * This file is part of Tuxcast, "The linux podcatcher"
 * Copyright (C) 2006-2008 David Turner
 * Copyright (C) 2010 Mathew Cucuzella (kookjr@gmail.com)
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



#ifndef TUXCAST_FUNCTIONS_H
#define TUXCAST_FUNCTIONS_H

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include "rss.h"
#include <map>

using namespace std;

#include "../config/config.h"

// Hooks:
enum {POSTDOWNLOAD, POSTRUN, INCORRECTMIME};

#ifdef THREADS
struct threaddata
{
	file thefile;
	filelist *allfiles;
	configuration *myconfig;
};
void* threadfunc(void* data);
#endif

void newfile(string name);
void newfiles(void);
bool alreadydownloaded(string name); // true if already downloaded
void check(configuration &myconfig, feed &feed, filelist &allfiles);
void up2date(configuration &myconfig, feed &feed, filelist &allfiles, int episodes);
void checkall(configuration &myconfig);
void up2dateall(configuration &myconfig, int episodes);
void get(file &file, configuration &myconfig);
void getlist(filelist &files, configuration &myconfig);
void populate_download_path(feed &feed, file &file, configuration &myconfig);
void handle_bittorrent (file &file);
void cachefeed(const string &name, const string &URL);
auto_ptr<filelist> parsefeed(string name);

void setvars(map<char,string> &vars, configuration &myconfig);
void runhook(int hook, map<char,string> vars, configuration &myconfig);
bool exclude_file(file &thefile);
int s_printf(const char* format, ...);
void show_episodes(configuration &myconfig, string &url, bool fetch_format);

#endif
