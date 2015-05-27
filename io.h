#ifndef IO_H
#define IO_H

#include <iostream>
#include <vector>

using namespace std;

class Flags{
public:
    static bool verbose;
    static bool pause;
    static bool error;

    static string version;
};

void show_help();

bool next_element(string element0, vector<string> *elements, int &next);

bool is_arg(string argument, vector<string> *args);

bool input_cmd();

#endif
