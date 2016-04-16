#include "log.h"

int Log::log_level = 3;
int Log::errors_count = 0;


void Log::error(string s){
    echo("[ERROR] " + s, 1);
    errors_count++;
}

void Log::warn(string s){
    echo("[warn] " + s, 2);
}

void Log::info(string s){
    echo(s, 3);
}

void Log::debug(string s){
    echo("[debug] " + s, 4);
}


void Log::echo(string s, int level){
    if(level > log_level) return;
    cout<<s<<endl;
    if(level == 1){ //ERROR
        cerr<<s<<endl;
    }
}
