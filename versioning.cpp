#include "versioning.h"
#include "files.h"
#include <fstream>
#include <sstream>
#include <cstring>

bool version_get(string filename, int &v1, int &v2, int &v3){
    if(!file_exists(filename)){
        cout<<"[!] BLAD: nie istnieje plik wersji: "<<filename<<endl;
        return false;
    }
    vector<string> *lines = get_nonempty_lines(filename);
    unsigned int liniav;
    stringstream ss;
    for(liniav=0; liniav<=lines->size(); liniav++){
        if(liniav==lines->size()){
            cout<<"[!] BLAD: brak linii z numerem wersji"<<endl;
            return false;
        }
        //szukanie formatu //v 1 2 13
        if(lines->at(liniav).length()<8)
            continue;
        if(lines->at(liniav)[0]=='/' && lines->at(liniav)[1]=='/' && lines->at(liniav)[2]=='v') break;
    }
    ss<<lines->at(liniav);
    char buf[5];
    buf[4] = 0;
    ss.read(buf,4);
    if(strcmp(buf,"//v ")!=0){
        cout<<"[!] BLAD: blad formatu danych w pliku wersji"<<endl;
        return false;
    }
    ss.str(ss.str().substr(4,ss.str().length()-4));
    ss>>v1>>v2>>v3;
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
    //zapis do pliku
    fstream plik;
    plik.open(filename.c_str(),fstream::out|fstream::binary);
    if(!plik.good()){
        cout<<"[!] BLAD: blad zapisu do pliku: "<<filename<<endl;
        return false;
    }
    plik<<"//v "<<v1<<" "<<v2<<" "<<v3<<endl;
    plik<<"// -- "<<get_time_date()<<" --"<<endl;
    plik<<"#ifndef VERSION_H"<<endl;
    plik<<"#define VERSION_H"<<endl<<endl;
    plik<<"#define VERSION \""<<v1<<"."<<v2<<"."<<v3<<"\""<<endl<<endl;
    plik<<"#endif"<<endl;
    plik.close();
    cout<<"Zwiekszono numer kolejnej wersji na: "<<v1<<"."<<v2<<"."<<v3<<endl;
    return true;
}
