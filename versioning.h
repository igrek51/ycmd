#ifndef VERSIONING_H
#define VERSIONING_H

#include <iostream>

using namespace std;

string get_time_date();

bool version_get(string filename, int &v1, int &v2, int &v3);
bool version_save(string filename, int v1, int v2, int v3);
bool version_inc(string filename);
bool init_version();

#endif
