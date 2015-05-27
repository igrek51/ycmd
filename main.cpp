#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <sstream>
#include <conio.h>
#include <fstream>

#include "system.h"
#include "commands.h"

using namespace std;

int main(int argc, char **argv){
    //pobranie argumentów do kontenera
    vector<string> *args = new vector<string>;
    for(int i=1; i<argc; i++){
        args->push_back(argv[i]);
	}
    //pomoc (brak parametru)
    if(is_arg("--help",args)||is_arg("-h",args)||args->size()==0){
        show_help();
		return 0;
	}
    //flagi verbose i pause
    if(is_arg("-v",args)) Flags::verbose = true;
    if(is_arg("-p",args)) Flags::pause = true;
    if(!exec_commands(args))
        Flags::error = true;
    //pause if error
    if(Flags::error && (is_arg("--pe",args)||is_arg("-pe",args))) Flags::pause = true;
    if(Flags::pause || Flags::verbose){
        if(Flags::error){
			cout<<"Program zakonczony bledem...";
		}else{
			cout<<"Pomyslnie zakonczono program...";
		}
        if(Flags::pause) getch();
	}
    delete args;
    if(Flags::error) return 1;
	return 0;
}
