#ifndef VARIABLE_H
#define VARIABLE_H

#include <iostream>
#include <vector>

using namespace std;

class Variable {
public:
    Variable(string name, string value);
    string name;
    string value;
};

vector<string>* get_variables_lines(string filename);

vector<Variable*>* get_variables(string filename);

string get_var_string(vector<Variable*>* variables, string name);
int get_var_int(vector<Variable*>* variables, string name);
bool get_var_bool(vector<Variable*>* variables, string name);

string trim_spaces(string s);

#endif
