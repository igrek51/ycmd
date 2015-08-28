#include "commands.h"
#include "files.h"
#include "io.h"
#include "system.h"
#include "versioning.h"
#include "ymake.h"

#include <cstdlib>
#include <ctime>
#include <sstream>

bool exec_commands(vector<string> *args){
    int next;
    string next_text1;
    string next_text2;
    //zmiana katalogu roboczego
    if(next_element("-w",args,next)||next_element("--workdir",args,next)){
        if(!set_workdir(dir_format(args->at(next)))){
            cout<<"[!] BLAD: nieprawidlowa sciezka \""<<dir_format(args->at(next))<<"\""<<endl;
            return false;
        }
    }
    if(next_element("--setenv",args,next)){ //zmienna środowiskowa
        if(next+1>=(int)args->size()){ //jeśli nie istnieje parametr value
            cout<<"[!] BLAD: brak wartosci zmiennej srodowiskowej"<<endl;
            return false;
        }
        if(!set_env(args->at(next),args->at(next+1))){
            cout<<"[!] BLAD: blad zmiany zmiennej srodowiskowej"<<endl;
            return false;
        }
    }
    if(next_element("--setpath",args,next)){ //zmienna środowiskowa PATH
        if(!set_env("PATH",args->at(next))){
            cout<<"[!] BLAD: blad zmiany zmiennej srodowiskowej"<<endl;
            return false;
        }
    }
    if(next_element("--addpath",args,next)){ //zmienna środowiskowa PATH
        if(!add_path(args->at(next))){
            cout<<"[!] BLAD: blad dodawania do zmiennej srodowiskowej PATH"<<endl;
            return false;
        }
    }
    //POLECENIA
    if(next_element("-e",args,next)||next_element("--exec",args,next)){ //wykonaj polecenia z parametrów
        for(unsigned int i=next; i<args->size(); i++){
            if(args->at(i)[0]=='-')
                continue; //omiń inne parametry
            if(!system2(args->at(i)))
                return false; //przerwij łancuch w przypadku błędu
        }
    }else if(next_element("-e1",args,next)){ //wykonaj 1 polecenie
        return system2(args->at(next));
    }else if(next_element("-f",args,next)||next_element("--file",args,next)){ //wykonaj wiersze z pliku
        if(Flags::verbose) cout<<"Wykonywanie zawartosci pliku \""<<args->at(next)<<"\":"<<endl;
        vector<string> *linie = get_nonempty_lines(args->at(next));
        if(linie==NULL){
            cout<<"[!] BLAD: brak pliku \""<<args->at(next)<<"\""<<endl;
            delete linie;
            return false;
        }
        for(unsigned int i=0; i<linie->size(); i++){
            if(!system2((*linie)[i])) break; //przerwij łancuch w przypadku błędu
        }
        delete linie;
    }else if(next_element("-t",args,next)||next_element("--time",args,next)){ //zmierz czas wykonania
        return count_time(args->at(next));
    }else if(next_element_default("-y", args, next_text1, "ymake") || next_element_default("--ymake", args, next_text1, "ymake")){ //skompiluj
        if(!ymake(dir_format(next_text1)))
            return false;
    }else if(next_element_default2("--ymake-bat", args, next_text1, "ymake", next_text2, "build.bat")){ //generuj plik .bat
        if(!ymake_generate_bat(dir_format(next_text1), dir_format(next_text2)))
            return false;
    }else if(next_element_default2("--ymake-makefile", args, next_text1, "ymake", next_text2, "Makefile")){ //generuj plik makefile
        if(!ymake_generate_makefile(dir_format(next_text1), dir_format(next_text2)))
            return false;
    }else if(next_element_default("--run", args, next_text1, "ymake")){ //uruchom aplikację z ymake
        if(!run_ymake(dir_format(next_text1), false)){
            cout<<"[!] BLAD: blad uruchamiania z pliku: "<<next_text1<<endl;
            return false;
        }
    }else if(next_element_default("--run-start", args, next_text1, "ymake")){ //uruchom aplikację z ymake (z poleceniem start)
        if(!run_ymake(dir_format(next_text1),true)){
            cout<<"[!] BLAD: blad uruchamiania z pliku: "<<next_text1<<endl;
            return false;
        }
    }else if(is_arg("-i",args)||is_arg("--input",args)){ //czekaj na wpisanie z klawiatury
        return input_cmd();
    }else if(is_arg("--clean",args)){ //wyczysc folder prv
        return clean_all();
    }else if(next_element_default("--version++", args, next_text1, "version.h")){ //zwiększ wersję w pliku
        return version_inc(dir_format(next_text1));
    }else{
        cout<<"[!] BLAD: brak poprawnego polecenia"<<endl;
        return false;
    }
    return true;
}

bool clean_all(){
    if(!dir_exists("prv")){
        cout<<"INFO: brak folderu prv\\"<<endl;
        return true;
    }
    cout<<"Czyszczenie..."<<endl;
    vector<string>* files = get_files_from_dir("prv");
    for(unsigned int i=0; i<files->size(); i++){
        cout<<"Usuwanie: prv\\"<<files->at(i)<<endl;
        if(!delete_file("prv\\"+files->at(i))){
            cout<<"[!] BLAD: blad usuwania pliku prv\\"<<files->at(i)<<endl;
            return false;
        }
    }
    return true;
}
