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

int main(void)
{
	cout << "Err, hello, world :)" << endl;
	pthread_t worker, manager;
	int result;
	interthreaddata data;

	cout << "Launching worker..." << endl;
	if(pthread_create(&worker,NULL,workerfunction, (void*)&data))
	{
		cout << "Couldn't spawn worker process: aborting" << endl;
		return -1;
	}
	else
		cout << "Worker is live" << endl;
	cout << "Launching manager..." << endl;
	if(pthread_create(&manager,NULL,managerfunction,(void*)&data))
	{
		cout << "Couldn't spawn manager process: aborting" << endl;
		return -1;
	}
	else
		cout << "Manager is live" << endl;

	while(true);

	return 0;
}

