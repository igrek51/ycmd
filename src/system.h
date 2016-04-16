#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>

using namespace std;

bool system2(string cmd);
bool system_echo(string cmd);

bool set_env(string variable, string value);
bool add_env_path(string new_path);

bool set_workdir(string wd);

string get_time();
string get_time_date();
bool count_time(string exec);

#endif
