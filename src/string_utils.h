#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <iostream>
#include <vector>

using namespace std;

string trim_spaces(string s);
string trim_quotes(string s);

vector<string>* get_param_list(string lista);

#endif
