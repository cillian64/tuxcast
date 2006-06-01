#ifndef TUXCAST_H
#define TUXCAST_H

#include <string>

using namespace std;

#include "../config/config.h"


void newfile(string name);
bool alreadydownloaded(string name); // true if already downloaded
void check(configuration *myconfig, int feed);
void up2date(configuration *myconfig, int feed);
void checkall(configuration *myconfig);
void up2dateall(configuration *myconfig);
void get(string name, string URL, int feed, configuration *myconfig);

#endif
