#include "io.h"
#include "system.h"
#include "version.h"

#include <conio.h>

bool IO::quiet = false;
bool IO::pause = false;
bool IO::error_was = false;
string IO::version = VERSION;

void IO::echo(string s, int priority){
    if(quiet && priority<=0) return;
    cout<<s<<endl;
}

void IO::error(string s){
    echo("[BLAD!] - "+s, 1);
    error_was = true;
}

void IO::info(string s){
    echo("[info] - "+s);
}

void show_help(){
    cout<<"\t\tycmd wersja "<<IO::version<<endl;
    cout<<"Polecenia:"<<endl;
    cout<<"ycmd -e(--exec) [polecenie1] [polecenie2] [...] - wykonaj polecenia"<<endl;
    cout<<"ycmd -1(--e1) [po le ce nie] - wykonaj jako jedno polecenie"<<endl;
    cout<<"ycmd -f(--file) [plik] - wykonaj wiersze z pliku (przerwij w przypadku bledu)"<<endl;
    cout<<"ycmd -y(--ymake) [ymake] - zbuduj na podstawie pliku ymake"<<endl;
    cout<<"ycmd --ymake-bat [ymake] [build.bat] - utworz plik .bat na podstawie pliku ymake"<<endl;
    cout<<"ycmd --ymake-makefile [ymake] [Makefile] - utworz plik Makefile na podstawie pliku ymake"<<endl;
    cout<<"ycmd -t(--time) [polecenie] - zmierz czas wykonania polecenia"<<endl;
    cout<<"ycmd -i(--input) - czekaj na wpisanie polecenia"<<endl;
    cout<<"ycmd --clean - wyczysc folder prv\\"<<endl;
    cout<<"ycmd --run [ymake] - uruchom z pliku ymake"<<endl;
    cout<<"ycmd --run-start [ymake] - uruchom poleceniem start z pliku ymake"<<endl;
    cout<<"ycmd --run-shell [ymake] - uruchom poleceniem ShellExecute z pliku ymake"<<endl;
    cout<<"ycmd --version++ [version.h] - zwieksz numer wersji w pliku"<<endl;
    cout<<"Opcjonalne parametry:"<<endl;
    cout<<"-w(--workdir) [dir] - zmien katalog roboczy"<<endl;
    cout<<"--setenv [var] [value] - ustaw zmienna srodowiskowa"<<endl;
    cout<<"--setpath [path] - ustaw zmienna srodowiskowa PATH"<<endl;
    cout<<"--addpath [path] - dodaj do zmiennej srodowiskowej PATH"<<endl;
    cout<<"-q - wyswietlaj tylko bledy"<<endl;
    cout<<"-p - czekaj na wcisniecie klawisza po zakonczeniu"<<endl;
    cout<<"--pe - czekaj na wcisniecie w przypadku bledu"<<endl;
}


vector<string>* get_args(int argc, char **argv){
    vector<string>* args = new vector<string>;
    for(int i=1; i<argc; i++){
        args->push_back(argv[i]);
	}
    return args;
}

int find_arg(string szukany, vector<string>* args){
    for(unsigned int i=0; i<args->size(); i++){
        if(args->at(i)==szukany) return i;
    }
    return -1;
}

bool is_arg(string szukany, vector<string> *args){
    return find_arg(szukany, args)!=-1;
}

bool next_arg_number(string szukany, vector<string>* args, int &next){
    int i = find_arg(szukany, args);
    if(i==-1) return false; //nie znalaz³o szukanego argumentu
    if(i+1 < (int)args->size()){ //jeœli istnieje nastêpny element
        next = i + 1;
        return true;
    }else{
        IO::error("Brak parametru po: "+szukany);
        return false; //nie istnieje nastêpny element
    }
}

bool next_arg(string szukany, vector<string>* args, string &next_text, string domyslny){
    int i = find_arg(szukany, args);
    if(i==-1) return false; //nie znalaz³o szukanego argumentu
    if(i+1 < (int)args->size()){ //jeœli istnieje nastêpny element
        next_text = args->at(i+1);
        return true;
    }else{ //nie istnieje nastêpny element
        if(domyslny.length()>0){ //zosta³a podana wartoœæ domyœlna
            next_text = domyslny;
            IO::info("Przyjeto domyslna wartosc 1. parametru po \""+szukany+"\": "+domyslny);
            return true;
        }else{ //brak zdefiniowanej wartoœci domyœlnej
            IO::error("Brak parametru po: "+szukany);
            return false;
        }
    }
}

bool next_arg2(string szukany, vector<string>* args, string &next_text1, string &next_text2, string domyslny1, string domyslny2){
    int i = find_arg(szukany, args);
    if(i==-1) return false; //nie znalaz³o szukanego argumentu
    if(i+2 < (int)args->size()){ //jeœli istniej¹ 2 nastêpne elementy
        next_text1 = args->at(i+1);
        next_text2 = args->at(i+2);
        return true;
    }else if(i+1 < (int)args->size()){ //istnieje 1 nastêpny element
        if(domyslny2.length()>0){ //zosta³a podana 2. wartoœæ domyœlna
            next_text1 = args->at(i+1);
            next_text2 = domyslny2;
            IO::info("Przyjeto domyslna wartosc 2. parametru po \""+szukany+"\": "+domyslny2);
            return true;
        }else{ //brak zdefiniowanych wartoœci domyœlnych
            IO::error("Brak drugiego parametru po "+szukany);
            return false; //nie istniej¹ oba
        }
    }else{ //nie istnieje ¿odyn nastêpny
        if(domyslny1.length()>0 && domyslny2.length()>0){ //podane obie wartoœci domyœlne
            next_text1 = domyslny1;
            next_text2 = domyslny2;
            IO::info("Przyjeto domyslna wartosc 1. parametru po \""+szukany+"\": "+domyslny1);
            IO::info("Przyjeto domyslna wartosc 2. parametru po \""+szukany+"\": "+domyslny2);
            return true;
        }else{ //brak zdefiniowanych wartoœci domyœlnych
            IO::error("Brak 2 parametrow po "+szukany);
            return false; //nie istniej¹ oba
        }
    }
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
