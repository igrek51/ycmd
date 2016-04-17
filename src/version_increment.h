#ifndef VERSION_INCREMENT_H
#define VERSION_INCREMENT_H

#include <iostream>

using namespace std;

string get_time_date();

bool version_get(string filename, int &v1, int &v2, int &v3);

bool version_save(string filename, int v1, int v2, int v3);

bool version_increment(string filename);

bool version_init();

#endif
