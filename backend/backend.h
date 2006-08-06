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

#ifndef BACKEND_H
#define BACKEND_H

#include "../tuxcast/tuxcast_functions.h"
#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <string>

using namespace std;

#define TYPE_STATUS

class interthreaddata
{
	public:
		interthreaddata()
		{ manager_attention = worker_attention = false;
		pthread_mutex_init(&mutex,NULL); }
		// Each thread should ONLY set data in it's section
		// i.e. the manager thread sets manager_*
		// And the worker sets worker_*
		// With the exception of _attention flags:
		// The other thread should change the first thread's
		// attention flag to false once it's got the data
		pthread_mutex_t mutex;

		/******************************************************/
		// This is the manager section:
		// This is where the manager sets details, i.e.
		// Which feed to check, etc.etc.
		bool manager_attention;

		/******************************************************/
		// This is the status section, i.e. this is where the
		// status reports from the worker go.
		// Current feed being checked (if any)
		bool worker_attention;
		string worker_feed;
};

#endif
