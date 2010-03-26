/*
 * This file is part of Tuxcast, "The linux podcatcher"
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

#ifndef EPISODE_HISTORY_H
#define EPISODE_HISTORY_H

#include <string>
#include <map>
#include <pthread.h>

class episode_history {

public:
    // use non-default file for loading and saving
    //   only valid before first call to getInstance
    static void set_filename(std::string path);
    // singleton accessor
    static episode_history* getInstance();

    // save history back out to disk
    bool flush(void);
    bool flush(std::string path);
    bool needs_flush();
    // add an episode to the history, given feed url and episode file name
    void add_episode(std::string url, std::string filename);
    // is this episode in the history, given feed url and episode file name
    bool exists(std::string url, std::string filename);

private:
    static episode_history* _instance;

    // ctor - empty history
    episode_history(void);
    // ctor - empty history
    ~episode_history(void);
    // copy ctor - restrict
    episode_history(const episode_history& other);
    // = operator - restrict
    episode_history& operator=(const episode_history& other);

    std::string default_file(void);
    void load(std::string path);    // load history from standard config file

    std::map<std::string, int> _history;
    pthread_mutex_t _lock;
    bool updated;
};

#endif /* EPISODE_HISTORY_H */

