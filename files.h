#ifndef FILES_H
#define FILES_H

#include <iostream>
#include <vector>

using namespace std;

bool file_exists(string filename);
bool dir_exists(string name);

bool files_equal(string file1, string file2);
bool copy_files(string file1, string file2);
bool delete_file(string filename);

vector<string>* get_all_lines(string filename);
vector<string>* get_nonempty_lines(string filename);

bool has_extension(string name, string ext);
string dir_format(string dir);

vector<string>* get_files_from_dir(string dir, string ext="");

string remove_file_extension(string filename);

#endif
