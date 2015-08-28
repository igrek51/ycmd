#include "io.h"
#include "system.h"
#include "version.h"
#include <conio.h>

bool Flags::verbose = false;
bool Flags::pause = false;
bool Flags::error = false;
string Flags::version = VERSION;

void show_help(){
    cout<<"Wersja "<<Flags::version<<endl;
    cout<<"Polecenia:"<<endl;
    cout<<"ycmd -e(--exec) [polecenie1] [polecenie2] [...] - wykonaj polecenia"<<endl;
    cout<<"ycmd -e1 [polecenie] - wykonaj jedno polecenie"<<endl;
    cout<<"ycmd -f(--file) [plik] - wykonaj wiersze z pliku (przerwij w przypadku bledu)"<<endl;
    cout<<"ycmd -y(--ymake) [ymake] - zbuduj na podstawie pliku ymake"<<endl;
    cout<<"ycmd --ymake-bat [ymake] [build.bat] - utworz plik .bat na podstawie pliku ymake"<<endl;
    cout<<"ycmd --ymake-makefile [ymake] [Makefile] - utworz plik Makefile na podstawie pliku ymake"<<endl;
    cout<<"ycmd -t(--time) [polecenie] - zmierz czas wykonania polecenia"<<endl;
    cout<<"ycmd -i(--input) - czekaj na wpisanie polecenia"<<endl;
    cout<<"ycmd --clean - wyczysc folder prv\\"<<endl;
    cout<<"ycmd --run [ymake] - uruchom z pliku ymake"<<endl;
    cout<<"ycmd --run-start [ymake] - uruchom poleceniem start z pliku ymake"<<endl;
    cout<<"ycmd --version++ [version.h] - zwieksz numer wersji w pliku"<<endl;
    cout<<"Opcjonalne parametry:"<<endl;
    cout<<"-w(--workdir) [dir] - zmien katalog roboczy"<<endl;
    cout<<"--setenv [var] [value] - zmien zmienna srodowiskowa"<<endl;
    cout<<"--setpath [path] - zmien zmienna srodowiskowa PATH"<<endl;
    cout<<"--addpath [path] - dodaj do zmiennej srodowiskowej PATH"<<endl;
    cout<<"-v - wyswietlaj wszystkie komunikaty"<<endl;
    cout<<"-p - czekaj na wcisniecie klawisza po zakonczeniu"<<endl;
    cout<<"--pe - czekaj w przypadku bledu"<<endl;
}

bool next_element(string szukany, vector<string>* elements, int &next){
    for(unsigned int i=0; i<elements->size(); i++){
        if((*elements)[i]==szukany){
            if(i+1<elements->size()){
                next = i+1;
                return true;
            }else{
                cout<<"[!] BLAD: brak parametru po "<<szukany<<endl;
                next = -2;
                Flags::error = true;
                return false; //nie istnieje następny
            }
        }
    }
    next = -1;
    return false; //nie znalazło argumentu
}

bool next_element_default(string szukany, vector<string>* elements, string &next_text, string domyslny){
    for(unsigned int i=0; i<elements->size(); i++){
        if(elements->at(i) == szukany){
            if(i+1 < elements->size()){ //istnieje następny element
                next_text = elements->at(i+1);
            }else{ //nie istnieje następny
                next_text = domyslny;
                cout<<"Domyslna wartosc 1. parametru po "<<szukany<<": "<<domyslny<<endl;
            }
            return true;
        }
    }
    return false; //nie znalazło szukanego argumentu
}

bool next_element_default2(string szukany, vector<string>* elements, string &next_text1, string domyslny1, string &next_text2, string domyslny2){
    for(unsigned int i=0; i<elements->size(); i++){
        if(elements->at(i) == szukany){
            if(i+2 < elements->size()){ //istnieją 2 następne elementy
                next_text1 = elements->at(i+1);
                next_text2 = elements->at(i+2);
            }else if(i+1 < elements->size()){ //istnieje 1 następny element
                next_text1 = elements->at(i+1);
                next_text2 = domyslny2;
                cout<<"Domyslna wartosc 2. parametru po "<<szukany<<": "<<domyslny2<<endl;
            }else{ //nie istnieje żodyn następny
                next_text1 = domyslny1;
                next_text2 = domyslny2;
                cout<<"Domyslna wartosc 1. parametru po "<<szukany<<": "<<domyslny1<<endl;
                cout<<"Domyslna wartosc 2. parametru po "<<szukany<<": "<<domyslny2<<endl;
            }
            return true;
        }
    }
    return false; //nie znalazło szukanego argumentu
}

bool is_arg(string argument, vector<string> *args){
    for(unsigned int i=0; i<args->size(); i++){
        if(args->at(i)==argument) return true;
    }
    return false;
}

bool input_cmd(){
    char ch, ch2;
    string cmd="";
    while(true){
        cout<<"> ";
        cmd="";
        do{
            ch=getch();
            if(ch==27){ //escape
                cmd="exit";
                break;
            }
            if(ch==8){ //backspace
                if(cmd.length()>0){
                    cmd=cmd.substr(0,cmd.length()-1);
                    continue;
                }
            }
            if(ch=='\r'||ch=='\n'){ //enter
                cout<<endl;
                break;
            }
            if(ch==-32){
                ch2=getch();
                continue;
            }
            cout<<ch;
            cmd+=ch;
        }while(ch!='\r'&&ch!='\n');
        if(cmd=="exit") break;
        system2(cmd);
        //cout<<endl;
    }
    return true;
}
