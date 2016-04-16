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
    echo("[ERROR!] - "+s, 1);
    error_was = true;
}

void IO::info(string s){
    echo("[info] - "+s);
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
    if(i==-1) return false; //nie znalazło szukanego argumentu
    if(i+1 < (int)args->size()){ //jeśli istnieje następny element
        next = i + 1;
        return true;
    }else{
        IO::error("Brak parametru po: "+szukany);
        return false; //nie istnieje następny element
    }
}

bool next_arg(string szukany, vector<string>* args, string &next_text, string domyslny){
    int i = find_arg(szukany, args);
    if(i==-1) return false; //nie znalazło szukanego argumentu
    if(i+1 < (int)args->size()){ //jeśli istnieje następny element
        next_text = args->at(i+1);
        return true;
    }else{ //nie istnieje następny element
        if(domyslny.length()>0){ //została podana wartość domyślna
            next_text = domyslny;
            IO::info("Przyjeto domyslna wartosc 1. parametru po \""+szukany+"\": "+domyslny);
            return true;
        }else{ //brak zdefiniowanej wartości domyślnej
            IO::error("Brak parametru po: "+szukany);
            return false;
        }
    }
}

bool next_arg2(string szukany, vector<string>* args, string &next_text1, string &next_text2, string domyslny1, string domyslny2){
    int i = find_arg(szukany, args);
    if(i==-1) return false; //nie znalazło szukanego argumentu
    if(i+2 < (int)args->size()){ //jeśli istnieją 2 następne elementy
        next_text1 = args->at(i+1);
        next_text2 = args->at(i+2);
        return true;
    }else if(i+1 < (int)args->size()){ //istnieje 1 następny element
        if(domyslny2.length()>0){ //została podana 2. wartość domyślna
            next_text1 = args->at(i+1);
            next_text2 = domyslny2;
            IO::info("Przyjeto domyslna wartosc 2. parametru po \""+szukany+"\": "+domyslny2);
            return true;
        }else{ //brak zdefiniowanych wartości domyślnych
            IO::error("Brak drugiego parametru po "+szukany);
            return false; //nie istnieją oba
        }
    }else{ //nie istnieje żodyn następny
        if(domyslny1.length()>0 && domyslny2.length()>0){ //podane obie wartości domyślne
            next_text1 = domyslny1;
            next_text2 = domyslny2;
            IO::info("Przyjeto domyslna wartosc 1. parametru po \""+szukany+"\": "+domyslny1);
            IO::info("Przyjeto domyslna wartosc 2. parametru po \""+szukany+"\": "+domyslny2);
            return true;
        }else{ //brak zdefiniowanych wartości domyślnych
            IO::error("Brak 2 parametrow po "+szukany);
            return false; //nie istnieją oba
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
