#ifndef PATH_H
#define PATH_H

#include <iostream>

using namespace std;

class Path {
private:
    string path;

    string getUnixPath();
    string getWindowsPath();

    static string cutSlashFromBeginning(string pathstr);
    static string cutSlashFromEnd(string pathstr);
    static string reformatSlash(string pathstr);

    static string replace(string str, char find, char replaceTo);
public:
    Path(string path);

    string getPath();

    bool endsWith(string ext);
    Path* removeExtension();

    Path* append(string child);
    Path* append(Path* child);

    static string reformat(string path);
    static string append(string dir, string filename);
    static string removeExtenstion(string filename);
};

#endif
