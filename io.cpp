#include "io.h"
#include "system.h"
#include "version.h"

#include <conio.h>

IO* IO::instance = NULL;

IO* IO::i(){
    if(instance == NULL){
        instance = new IO();
    }
    return instance;
}

void IO::echo(string s){
    cout<<s<<endl;
}

void IO::error(string s){
    echo("[BLAD!] - "+s);
}

void IO::info(string s){
    echo("[info] - "+s);
}

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

bool next_arg(string szukany, vector<string>* args, string &next_text){
    for(unsigned int i=0; i<args->size(); i++){
        if(args->at(i) == szukany){
            if(i+1 < args->size()){ //jeœli istnieje nastêpny element
                next_text = args->at(i+1);
                return true;
            }else{
                IO::error("brak parametru po "+szukany);
                Flags::error = true;
                return false; //nie istnieje nastêpny
            }
        }
    }
    return false; //nie znalaz³o argumentu
}

bool next_arg(string szukany, vector<string>* args, string &next_text1, string &next_text2){
    for(unsigned int i=0; i<args->size(); i++){
        if(args->at(i) == szukany){
            if(i+2 < args->size()){ //jeœli istniej¹ 2 nastêpne elementy
                next_text1 = args->at(i+1);
                next_text2 = args->at(i+2);
                return true;
            }else{
                IO::error("brak 2 parametrow po "+szukany);
                Flags::error = true;
                return false; //nie istniej¹ oba
            }
        }
    }
    return false; //nie znalaz³o argumentu
}

bool next_arg_default(string szukany, vector<string>* args, string &next_text, string domyslny){
    for(unsigned int i=0; i<args->size(); i++){
        if(args->at(i) == szukany){
            if(i+1 < args->size()){ //istnieje nastêpny element
                next_text = args->at(i+1);
            }else{ //nie istnieje nastêpny
                next_text = domyslny;
                IO::info("Domyslna wartosc 1. parametru po \""+szukany+"\": "+domyslny);
            }
            return true;
        }
    }
    return false; //nie znalaz³o szukanego argumentu
}

bool next_arg_default(string szukany, vector<string>* args, string &next_text1, string domyslny1, string &next_text2, string domyslny2){
    for(unsigned int i=0; i<args->size(); i++){
        if(args->at(i) == szukany){
            if(i+2 < args->size()){ //istniej¹ 2 nastêpne elementy
                next_text1 = args->at(i+1);
                next_text2 = args->at(i+2);
            }else if(i+1 < args->size()){ //istnieje 1 nastêpny element
                next_text1 = args->at(i+1);
                next_text2 = domyslny2;
                IO::info("Domyslna wartosc 2. parametru po \""+szukany+"\": "+domyslny2);
            }else{ //nie istnieje ¿odyn nastêpny
                next_text1 = domyslny1;
                next_text2 = domyslny2;
                IO::info("Domyslna wartosc 1. parametru po \""+szukany+"\": "+domyslny1);
                IO::info("Domyslna wartosc 2. parametru po \""+szukany+"\": "+domyslny2);
            }
            return true;
        }
    }
    return false; //nie znalaz³o szukanego argumentu
}

bool next_arg_number(string szukany, vector<string>* args, int &next){
    for(unsigned int i=0; i<args->size(); i++){
        if(args->at(i) == szukany){
            if(i+1 < args->size()){ //jeœli istnieje nastêpny element
                next = i + 1;
                return true;
            }else{
                IO::error("brak parametru po: "+szukany);
                Flags::error = true;
                return false; //nie istnieje nastêpny
            }
        }
    }
    return false; //nie znalaz³o argumentu
}

bool is_arg(string argument, vector<string> *args){
    for(unsigned int i=0; i<args->size(); i++){
        if(args->at(i)==argument) return true;
    }
    return false;
}

bool input_cmd(){
    char ch, ch2;
    string cmd = "";
    while(true){
        cout<<"> ";
        cmd = "";
        do{
            ch = getch();
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
