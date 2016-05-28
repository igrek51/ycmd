#ifndef YMAKE_DATA_SOURCE_H
#define YMAKE_DATA_SOURCE_H

#include <iostream>
#include <vector>

using namespace std;

class YmakeDataSource {
public:
    string compiler;
    string src;
    string headers;
    string src_path;
    string output;
    string libs;
    string compiler_flags;
    string linker_flags;
    string resource;
    string version_file;

    YmakeDataSource(string filename);

    bool validate();

    vector<string> *getSources();

    vector<string> *getHeaders();
};

#endif
