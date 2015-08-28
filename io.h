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

bool next_arg(string szukany, vector<string>* args, string &next_text);
bool next_arg(string szukany, vector<string>* args, string &next_text1, string &next_text2);
bool next_arg_default(string szukany, vector<string>* args, string &next_text, string domyslny="");
bool next_arg_default(string szukany, vector<string>* args, string &next_text1, string domyslny1, string &next_text2, string domyslny2="");
bool next_arg_number(string szukany, vector<string>* args, int &next);

bool is_arg(string argument, vector<string> *args);

bool input_cmd();

#endif
