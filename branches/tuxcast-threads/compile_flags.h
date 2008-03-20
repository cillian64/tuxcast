/*
 * 
 * This file is part of Tuxcast, "The linux podcatcher"
 * Copyright (C) 2006-2008 David Turner
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


// ONLY UNCOMMENT ONE OF THESE!:
#define POSIX
// #define BOOST
// #define BSD



// Uncomment for PCREPP-requiring features:
// Also, remember to comment or uncomment PCRE compile flags in config.mk
//#define PCREPP

// Uncomment for torrent support
// Also comment or uncomment libtorrent stuff in config.mk
#define TORRENT

// Uncomment for thread support:
#define THREADS
