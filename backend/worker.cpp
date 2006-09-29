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


#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <string>

#include "backend.h"
#include "worker.h"
#include "manager.h"

using namespace std;

void *workerfunction(void *data)
{
	interthreaddata *mydata=(interthreaddata*)data;
	while(true)
	{
		pthread_mutex_lock(&mydata->mutex);
		// Doesn't matter if this blocks, since they shouldn't
		// take too long to set up data etc...
		// TODO: Convert this to trylock, so we can get on
		// with our work
		if(mydata->manager_attention)
		{
			mydata->worker_feed="Mychin";
			mydata->worker_attention=true;
			mydata->manager_attention=false;
		}
		pthread_mutex_unlock(&mydata->mutex);
	}
}

