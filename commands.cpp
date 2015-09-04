#include "commands.h"
#include "files.h"
#include "io.h"
#include "system.h"
#include "versioning.h"
#include "ymake.h"

#include <sstream>

bool exec_commands(vector<string> *args){
    string next1;
    string next2;
    int next;
    //zmiana katalogu roboczego
    if(next_arg("-w", args, next1) || next_arg("--workdir", args, next1)){
        if(!set_workdir(next1)){
            IO::error("nieprawidlowa sciezka \""+next1+"\"");
            return false;
        }
    }
    if(next_arg("--setenv", args, next1, next2)){ //zmienna �rodowiskowa
        return set_env(next1, next2);
    }
    if(next_arg("--setpath", args, next1)){ //zmienna �rodowiskowa PATH
        return set_env("PATH", next1);
    }
    if(next_arg("--addpath", args, next1)){ //zmienna �rodowiskowa PATH
        if(!add_path(next1)){
            IO::error("blad dodawania do zmiennej srodowiskowej PATH");
            return false;
        }
    }
    //POLECENIA
    if(next_arg_number("-e", args, next) || next_arg_number("--exec", args, next)){ //wykonaj polecenia z parametr�w
        for(unsigned int i=next; i<args->size(); i++){
            if(!system_echo(args->at(i)))
                return false; //przerwij �ancuch w przypadku b��du
        }
    }else if(next_arg_number("-1", args, next) || next_arg_number("--e1", args, next)){ //wykonaj jako 1 polecenie
        //z�o�enie argument�w w jedno polecenie
        stringstream ss;
        for(unsigned int i=next; i<args->size(); i++){
            ss<<args->at(i);
            if(i<args->size()-1) ss<<" ";
        }
        return system_echo(ss.str());
    }else if(next_arg("-f", args, next1) || next_arg("--file", args, next1)){ //wykonaj wiersze z pliku
        IO::echo("Wykonywanie zawartosci pliku \""+next1+"\":");
        vector<string> *linie = get_nonempty_lines(next1);
        if(linie==NULL){
            IO::error("brak pliku \""+next1+"\"");
            delete linie;
            return false;
        }
        for(unsigned int i=0; i<linie->size(); i++){
            if(!system_echo(linie->at(i))){ //przerwij �ancuch w przypadku b��du
                delete linie;
                return false;
            }
        }
        delete linie;
    }else if(next_arg("-t", args, next1) || next_arg("--time", args, next1)){ //zmierz czas wykonania
        return count_time(next1);
    }else if(next_arg("-y", args, next1, "ymake") || next_arg("--ymake", args, next1, "ymake")){ //skompiluj
        if(!ymake(next1))
            return false;
    }else if(next_arg2("--ymake-bat", args, next1, next2, "ymake", "build.bat")){ //generuj plik .bat
        if(!ymake_generate_bat(next1, next2))
            return false;
    }else if(next_arg2("--ymake-makefile", args, next1, next2, "ymake", "Makefile")){ //generuj plik makefile
        if(!ymake_generate_makefile(next1, next2))
            return false;
    }else if(next_arg("--run", args, next1, "ymake")){ //uruchom aplikacj� z ymake
        if(!run_ymake(next1, 1)){
            IO::error("blad uruchamiania z pliku: "+next1);
            return false;
        }
    }else if(next_arg("--run-start", args, next1, "ymake")){ //uruchom aplikacj� z ymake (z poleceniem start)
        if(!run_ymake(next1, 2)){
            IO::error("blad uruchamiania z pliku: "+next1);
            return false;
        }
    }else if(next_arg("--run-shell", args, next1, "ymake")){ //uruchom aplikacj� przez shellexecute
        if(!run_ymake(next1, 3)){
            IO::error("blad uruchamiania z pliku: "+next1);
            return false;
        }
    }else if(is_arg("--init-ymake", args)){
        return init_ymake();
    }else if(is_arg("--init-version", args)){
        return init_version();
    }else if(is_arg("--init", args)){
        bool wynik1 = init_ymake();
        bool wynik2 = init_version();
        return wynik1 && wynik2;
    }else if(is_arg("-i", args) || is_arg("--input", args)){ //czekaj na wpisanie z klawiatury
        return input_cmd();
    }else if(is_arg("--clean", args)){ //wyczysc folder prv
        return clean_all();
    }else if(next_arg("--version++", args, next1, "version.h")){ //zwi�ksz wersj� w pliku
        return version_inc(dir_format(next1));
    }else if(is_arg("--test", args)){

    }else{
        IO::error("brak poprawnego polecenia");
        return false;
    }
    return true;
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
    cout<<"ycmd --init - utworz plik ymake oraz plik wersji version.h"<<endl;
    cout<<"ycmd --init-ymake - utworz plik ymake"<<endl;
    cout<<"ycmd --init-version - utworz plik wersji version.h"<<endl;
    cout<<"Opcjonalne parametry:"<<endl;
    cout<<"-w(--workdir) [dir] - zmien katalog roboczy"<<endl;
    cout<<"--setenv [var] [value] - ustaw zmienna srodowiskowa"<<endl;
    cout<<"--setpath [path] - ustaw zmienna srodowiskowa PATH"<<endl;
    cout<<"--addpath [path] - dodaj do zmiennej srodowiskowej PATH"<<endl;
    cout<<"-q - wyswietlaj tylko bledy"<<endl;
    cout<<"-p - czekaj na wcisniecie klawisza po zakonczeniu"<<endl;
    cout<<"--pe - czekaj na wcisniecie w przypadku bledu"<<endl;
}
