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



#ifndef SOCKET_EXCEPTIONS_H
#define SOCKET_EXCEPTIONS_H

#include <iostream>
#include "../exceptions.h"
using namespace std;

// The vanilla exception is now global to everything
// So it's now in ../exceptions.h

class eSocket_CannotOpenSocket : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Cannot open socket" << endl; }
};
class eSocket_CannotConnect : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Cannot connect" << endl; }
};

class eSocket_AlreadyConnected : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Socket already connected" << endl; }
};

class eSocket_CannotResolve : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Cannot resolve hostname" << endl; }
};

class eSocket_CannotBind : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Cannot bind to port" << endl;}
};

class eSocket_CannotListen : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Cannot listen on port" << endl; }
};

class eSocket_CannotAccept : public eException
{
	public:
		virtual void print(void)
		{ cerr << "Cannot accept connection" << endl; }
};

#endif
