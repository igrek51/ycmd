#include "commands.h"
#include "io.h"

#include <iostream>
#include <vector>
#include <conio.h>

using namespace std;

int main(int argc, char **argv){
    //pobranie argumentów do kontenera
    vector<string>* args = get_args(argc, argv);
    //pomoc (brak parametru)
    if(is_arg("--help", args) || is_arg("-h", args) || args->size()==0){
        show_help();
		return 0;
	}
    //flagi quiet i pause
    if(is_arg("-q", args)) IO::quiet = true;
    if(is_arg("-p", args)) IO::pause = true;
    //wykonywanie polecenia
    if(!exec_commands(args))
        IO::error_was = true;
    //pause if error
    if(IO::error_was && (is_arg("--pe", args) || is_arg("-pe", args))) IO::pause = true;
    if(IO::pause){
        if(IO::error_was){
			IO::echo("Program zakonczony bledem...", 1);
		}else{
			IO::echo("Pomyslnie zakonczono program...");
		}
        getch();
	}
    delete args;
    if(IO::error_was) return 1;
	return 0;
}
