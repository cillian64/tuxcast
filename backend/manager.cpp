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


#include "../tuxcast/tuxcast_functions.h"
#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <string>

#include "backend.h"
#include "worker.h"
#include "manager.h"

using namespace std;

void *managerfunction(void *data)
{
	interthreaddata *mydata;
	mydata = (interthreaddata*)data; 
	// This new pointer is a tad easier to access:
	// Less casting
	
	while(true)
	{
		// We sleep 5, fire for a status report,
		// then print the status report result.
		// Then we sleep forever.
		sleep(5);
		cout << "Sleep expired, sending ping" << endl;
		pthread_mutex_lock(&mydata->mutex);
		mydata->manager_attention=true;
		// Here we'd set any other more important info...
		pthread_mutex_unlock(&mydata->mutex);
		cout << "Sent status request" << endl;
		// Now we loop, checking for return:
		while(true)
		{
			pthread_mutex_lock(&mydata->mutex);
			if(mydata->worker_attention)
			{
				cout << "Status recieved: " << mydata->worker_feed << endl;
				mydata->worker_attention=false;
				pthread_mutex_unlock(&mydata->mutex);
				break;
			}
			pthread_mutex_unlock(&mydata->mutex);
		}
	}
}

