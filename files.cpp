#include "files.h"

#include <fstream>
#include <windows.h>

bool file_exists(string filename){
    fstream plik;
    plik.open(filename.c_str(),fstream::in|fstream::binary);
    if(plik.good()){
        plik.close();
        return true;
    }else{
        plik.close();
        return false;
    }
}

bool dir_exists(string name){
    DWORD ftyp = GetFileAttributesA(name.c_str());
    if(ftyp==INVALID_FILE_ATTRIBUTES) return false;
    if(ftyp&FILE_ATTRIBUTE_DIRECTORY) return true;
    return false;
}


bool files_equal(string file1, string file2){
    if(!file_exists(file1))	return false;
    if(!file_exists(file2))	return false;
    fstream plik1, plik2;
    //rozmiary plików
    plik1.open(file1.c_str(), fstream::in|fstream::binary);
    plik1.seekg(0, plik1.end);
    unsigned int fsize1 = plik1.tellg();
    plik2.open(file2.c_str(), fstream::in|fstream::binary);
    plik2.seekg(0, plik2.end);
    unsigned int fsize2 = plik2.tellg();
    if(fsize1!=fsize2){ //różne rozmiary
        plik1.close();
        plik2.close();
        return false;
    }
    //zawartość plików
    char *plik1_tab = new char [fsize1];
    plik1.seekg(0, plik1.beg);
    plik1.read(plik1_tab, fsize1);
    plik1.close();
    char *plik2_tab = new char [fsize2];
    plik2.seekg(0, plik2.beg);
    plik2.read(plik2_tab, fsize2);
    plik2.close();
    if(memcmp(plik1_tab, plik2_tab, fsize1)!=0){ //różna zawartość
        delete[] plik1_tab;
        delete[] plik2_tab;
        return false;
    }
    delete[] plik1_tab;
    delete[] plik2_tab;
    return true;
}

bool copy_files(string file1, string file2){
    return CopyFile(file1.c_str(),file2.c_str(),false);
}

bool delete_file(string filename){
    return DeleteFile(filename.c_str());
}


vector<string>* get_all_lines(string filename){
    fstream plik;
    plik.open(filename.c_str(),fstream::in|fstream::binary);
    if(!plik.good()){
        plik.close();
        return NULL;
    }
    vector<string>* lines = new vector<string>;
    string linia;
    do{
        getline(plik,linia,'\n'); //rozdzielenie znakami \n
        for(unsigned int i=0; i<linia.length(); i++){ //usunięcie znaków \r
            if(linia[i]=='\r'){
                linia.erase(linia.begin()+i);
                i--;
            }
        }
        lines->push_back(linia);
    }while(!plik.eof());
    plik.close();
    return lines;
}

vector<string>* get_nonempty_lines(string filename){
    filename = dir_format(filename);
    vector<string>* lines = get_all_lines(filename);
    if(lines==NULL) return NULL;
    for(unsigned int i=0; i<lines->size(); i++){
        if(lines->at(i).length()==0){ //usunięcie pustych elementów
            lines->erase(lines->begin()+i);
            i--;
        }
    }
    return lines;
}


bool has_extension(string name, string ext){
    if(ext.length()==0) return true;
    if(ext.length() > name.length()) return false;
    name = name.substr(name.length()-ext.length(),ext.length());
    if(name==ext) return true;
    return false;
}

string dir_format(string dir){
    for(unsigned int i=0; i<dir.length(); i++){
        if(dir[i]=='/') dir[i]='\\';
    }
    return dir;
}


vector<string>* get_files_from_dir(string dir, string ext){
    if(ext=="*")
        ext = "";
    if(dir.length()==0)
        dir = ".";
    if(!dir_exists(dir)){
        cout<<"[!] BLAD: brak folderu "<<dir<<endl;
        return NULL;
    }
    WIN32_FIND_DATAA ffd;
    HANDLE hFind = FindFirstFileA((dir+"\\*").c_str(), &ffd);
    if(hFind==INVALID_HANDLE_VALUE){
        cout<<"[!] BLAD: blad otwierania folderu "<<dir<<endl;
        return NULL;
    }
    vector<string>* files = new vector<string>;
    do{
        const char *stemp = string(ffd.cFileName).c_str();
        if(strcmp(stemp,".")==0) continue;
        if(strcmp(stemp,"..")==0) continue;
        if(strcmp(stemp,"desktop.ini")==0) continue;
        if(!(ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)){ //plik
            if(has_extension(ffd.cFileName, ext)){
                files->push_back(ffd.cFileName);
            }
        }
    }while(FindNextFileA(hFind, &ffd)!=0);
    FindClose(hFind);
    return files;
}
