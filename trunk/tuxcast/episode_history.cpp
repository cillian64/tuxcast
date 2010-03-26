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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

#include "episode_history.h"

/* -=-=-=-=-=  static part of class -=-=-=-=-= */

static pthread_mutex_t instance_lock = PTHREAD_MUTEX_INITIALIZER;
static std::string selected_filename = "";

episode_history* episode_history::_instance = NULL;

void episode_history::set_filename(std::string path) {
    pthread_mutex_lock(&instance_lock);
    {
        // only allow setting before instance created
        if (! _instance) {
            selected_filename = path;
        }
    }
    pthread_mutex_unlock(&instance_lock);
}

episode_history* episode_history::getInstance() {
    pthread_mutex_lock(&instance_lock);
    {
        if (! _instance) {
            _instance = new episode_history();
            if (selected_filename == "") {
                selected_filename = _instance->default_file();
            }
            _instance->load(selected_filename);
        }
    }
    pthread_mutex_unlock(&instance_lock);

    return _instance;
}

/* -=-=-=-=-=  instance part of class -=-=-=-=-= */

episode_history::episode_history(void) :
    _history(),
    updated(false)
{
    pthread_mutex_init(&_lock, NULL);
}

episode_history::~episode_history(void) {
    pthread_mutex_destroy(&_lock);
}

episode_history::episode_history(const episode_history& other) {
}

episode_history& episode_history::operator=(const episode_history& other) {
}

void episode_history::load(std::string path) {
    struct stat st;
    if (stat(path.c_str(), &st) == -1)
        return;  // starting from scratch

    xmlDoc *doc;
    xmlNode *root,*curr;

    doc = xmlReadFile(path.c_str(), NULL, 0);

    if (NULL == doc) {
        return;
    }
    root = xmlDocGetRootElement(doc);
    curr = root->children;

    while (curr) {
   	if (strcasecmp((char*)curr->name, "file") == 0) {
            if(curr->children != NULL) {
                _history[std::string((char *)curr->children->content)] = 0;
            }
        }
        curr = curr->next;
    }
    xmlFreeDoc(doc);
}

bool episode_history::flush(void) {
    return flush(selected_filename);
}

bool episode_history::flush(std::string path) {
    if (! updated) {
        return true;
    }

    xmlDoc *doc;
    xmlNode *root, curr;
    bool ret = true;

    doc = xmlNewDoc((xmlChar *)"1.0");
    root = xmlNewNode(NULL,(xmlChar *)"filelist");
    xmlDocSetRootElement(doc, root);

    std::map<std::string, int>::iterator it;
    for (it = _history.begin(); it != _history.end(); it++) {
        xmlNewChild(root, NULL, (xmlChar *)"file", (xmlChar *)it->first.c_str());
    }

    if (xmlSaveFormatFileEnc(path.c_str(), doc, "UTF-8", 1) == -1) {
        ret = false;
    }

    xmlFreeDoc(doc);

    return ret;
}

void episode_history::add_episode(std::string url, std::string filename) {
    pthread_mutex_lock(&_lock);
    {
        // ok to possibly overwrite existing k/v because value is not used
        _history[filename] = 0;
        updated = true;
    }
    pthread_mutex_unlock(&_lock);
}

bool episode_history::exists(std::string url, std::string filename) {
    bool ret = false;

    pthread_mutex_lock(&_lock);
    {
        std::map<std::string, int>::const_iterator it;
        it = _history.find(filename);
        if (it != _history.end()) {
            ret = true;
        }
    }
    pthread_mutex_unlock(&_lock);

    return ret;
}

bool episode_history::needs_flush() {
    return updated;
}

std::string episode_history::default_file(void) {
    std::string path = getenv("HOME");
    path += "/.tuxcast/files.xml";
    return path;
}
