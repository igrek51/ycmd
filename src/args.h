#ifndef ARGS_H
#define ARGS_H

#include "log.h"

#include <iostream>
#include <vector>

using namespace std;

class IO {
public:
    static bool pause;
    static string version;
};

vector<string> *get_args(int argc, char **argv);

int find_arg(string szukany, vector<string> *args);

bool is_arg(string szukany, vector<string> *args);

bool next_arg_number(string szukany, vector<string> *args, int &next);

//wyciągnięcie następnego parametru
bool next_arg(string szukany, vector<string> *args, string &next_text, string domyslny = "");

//wyciągnięcie 2 następnych parametrów
bool next_arg2(string szukany, vector<string> *args, string &next_text1, string &next_text2,
               string domyslny1 = "", string domyslny2 = "");

#endif
