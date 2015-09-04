#ifndef IO_H
#define IO_H

#include <iostream>
#include <vector>

using namespace std;

class IO{
public:
    static bool quiet;
    static bool pause;
    static bool error_was;
    static string version;
    static void echo(string s, int priority = 0);
    static void error(string s);
    static void info(string s);
};

vector<string>* get_args(int argc, char **argv);
int find_arg(string szukany, vector<string>* args);
bool is_arg(string szukany, vector<string> *args);
bool next_arg_number(string szukany, vector<string>* args, int &next);
//wyci�gni�cie nast�pnego parametru
bool next_arg(string szukany, vector<string>* args, string &next_text, string domyslny = "");
//wyci�gni�cie 2 nast�pnych parametr�w
bool next_arg2(string szukany, vector<string>* args, string &next_text1, string &next_text2, string domyslny1 = "", string domyslny2 = "");

bool input_cmd();

#endif
