/*
 * 
 * This file is part of Tuxcast, "The linux podcatcher"
 * Copyright (C) 2006 David Turner
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

#include <string>
#include "rss.h"

using namespace std;

#include "../config/config.h"


void newfile(string name);
bool alreadydownloaded(string name); // true if already downloaded
void check(configuration *myconfig, int feed);
void up2date(configuration *myconfig, int feed);
void checkall(configuration *myconfig);
void up2dateall(configuration *myconfig);
void get(string name, string URL, int feed, configuration *myconfig);
filelist *parsefeed(string URL);

#endif
