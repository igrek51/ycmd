#ifndef YMAKE_H
#define YMAKE_H

#include <iostream>

using namespace std;

bool ymake(string ymake_filename);

bool ymake_generate_bat(string ymake_filename, string output_filename);
bool ymake_generate_makefile(string ymake_filename, string output_filename);

bool run_ymake(string ymake_filename, int mode);

bool clean_all();

#endif
