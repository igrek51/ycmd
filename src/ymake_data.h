#ifndef YMAKE_DATA_H
#define YMAKE_DATA_H

#include <iostream>
#include <vector>

using namespace std;

class YmakeData {
public:
    string compiler;
    string compiler_path;
    string src;
    string headers;
    string src_path;
    string output;
    string libs;
    string compiler_flags;
    string linker_flags;
    string resource;
    string version_file;

    YmakeData(string filename);
    bool validate();
    vector<string>* getSources();
    vector<string>* getHeaders();
};

#endif
