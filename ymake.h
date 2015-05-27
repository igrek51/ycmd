#ifndef YMAKE_H
#define YMAKE_H

#include <iostream>
#include <vector>

#include "system.h"

using namespace std;

vector<string>* get_ymake_lines(string filename);

void add_to_list(vector<string>* kontener, string elem);
vector<string>* get_from_list(string list);
vector<string>* get_from_list_ex(string list, string dir);

bool ymake(string ymake_filename);
bool ymake_generate(string ymake_filename, string output_filename);
bool run_ymake(string ymake_filename, bool start);

#endif // YMAKE_H
