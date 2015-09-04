#include "versioning.h"
#include "files.h"
#include "io.h"

#include <fstream>
#include <sstream>
#include <cstring>

bool version_get(string filename, int &v1, int &v2, int &v3){
    if(!file_exists(filename)){
        IO::error("nie istnieje plik wersji: "+filename);
        return false;
    }
    vector<string> *lines = get_nonempty_lines(filename);
    unsigned int liniav;
    string pattern = "#define VERSION \"";
    for(liniav=0; liniav<=lines->size(); liniav++){
        if(liniav==lines->size()){
            IO::error("brak linii z numerem wersji");
            delete lines;
            return false;
        }
        //szukanie odpowiedniego formatu
        if(lines->at(liniav).length() < pattern.length()+6) continue;
        if(lines->at(liniav).substr(0, pattern.length()) == pattern) break;
    }
    stringstream ss;
    ss<<lines->at(liniav).substr(pattern.length());
    delete lines;
    ss>>v1;
    char bufor;
    ss.read(&bufor, 1);
    if(bufor!='.'){
        IO::error("blad formatu danych w pliku wersji");
        return false;
    }
    ss>>v2;
    ss.read(&bufor, 1);
    if(bufor!='.'){
        IO::error("blad formatu danych w pliku wersji");
        return false;
    }
    ss>>v3;
    ss.read(&bufor, 1);
    if(bufor!='\"'){
        IO::error("blad formatu danych w pliku wersji");
        return false;
    }
    ss>>v1>>v2>>v3;
    return true;
}

bool version_save(string filename, int v1, int v2, int v3){
    //zapis do pliku
    fstream plik;
    plik.open(filename.c_str(),fstream::out|fstream::binary);
    if(!plik.good()){
        IO::error("blad zapisu do pliku: "+filename);
        return false;
    }
    plik<<"// -- "<<get_time_date()<<" -- (ymake v"<<IO::version<<") --"<<endl;
    plik<<"#ifndef VERSION_H"<<endl;
    plik<<"#define VERSION_H"<<endl<<endl;
    plik<<"#define VERSION \""<<v1<<"."<<v2<<"."<<v3<<"\""<<endl<<endl;
    plik<<"#endif"<<endl;
    plik.close();
    return true;
}

bool version_inc(string filename){
    int v1=1, v2=0, v3=0;
    //pobranie numeru wersji
    if(!version_get(filename, v1, v2, v3))
        return false;
    //zwiêkszenie wersji
    v3++;
    if(v3>=100){
        v3 = 0;
        v2++;
    }
    if(v2>=100){
        v2 = 0;
        v1++;
    }
    if(version_save(filename, v1, v2, v3)){
        stringstream ss;
        ss<<"Zwiekszono numer kolejnej wersji na: "<<v1<<"."<<v2<<"."<<v3;
        IO::echo(ss.str());
        return true;
    }else{
        return false;
    }
}

bool init_version(){
    if(version_save("version.h", 1, 0, 1)){
        IO::echo("Utworzono nowy plik wersji: version.h");
        return true;
    }else{
        return false;
    }
}
