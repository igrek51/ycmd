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

bool next_element(string szukany, vector<string>* elements, int &next);
bool next_element_default(string szukany, vector<string>* elements, string &next_text, string domyslny="");
bool next_element_default2(string szukany, vector<string>* elements, string &next_text1, string domyslny1, string &next_text2, string domyslny2="");

bool is_arg(string argument, vector<string> *args);

bool input_cmd();

#endif
