#ifndef YMAKE_H
#define YMAKE_H

#include <iostream>
#include "ymake_data_source.h"

using namespace std;

bool ymake(string ymake_filename);

bool ymake_analyze_headers(YmakeDataSource *ds, bool &rebuild);

bool ymake_analyze_srcs(YmakeDataSource *ds, bool &rebuild, vector<string> *objs);

bool ymake_analyze_resources(YmakeDataSource *ds, bool &rebuild, vector<string> *objs);

bool ymake_version_increment(YmakeDataSource *ds);

bool ymake_linker_build(YmakeDataSource *ds, vector<string> *objs);

bool ymake_generate_bash(string ymake_filename, string output_filename);

bool ymake_generate_makefile(string ymake_filename, string output_filename);

bool run_from_ymake(string ymake_filename, int mode);

bool clean_all();

bool clean_dir(string dir);

bool ymake_init();

#endif
