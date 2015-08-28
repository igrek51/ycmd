#include "system.h"
#include "files.h"

#include <windows.h>
#include <ctime>
#include <sstream>

bool system2(string cmd){
    if(Flags::verbose)
        cout<<"Wykonywanie: "<<cmd<<" ..."<<endl;
    if(system(cmd.c_str())==0)
        return true;
    cout<<"[!] Blad polecenia: "<<cmd<<endl;
    Flags::error = true;
    return false;
}

bool set_env(string variable, string value){
    if(SetEnvironmentVariable(variable.c_str(),value.c_str())==0){
        cout<<"[BLAD!] Blad zmiany zmiennej srodowiskowej: "<<variable<<endl;
        return false;
    }
    return true;
}

bool add_path(string new_path){
    char *env_path = new char [1024];
    if(GetEnvironmentVariable("PATH",env_path,1024)==0){
        delete env_path;
        return false;
    }
    new_path = new_path + ";" + env_path;
    delete env_path;
    if(!set_env("PATH",new_path)) return false;
    return true;
}

bool set_workdir(string wd){
    wd = dir_format(wd);
    if(!SetCurrentDirectory(wd.c_str())){
        cout<<"[BLAD!] Blad zmiany katalogu roboczego: "<<wd<<endl;
        return false;
    }
    return true;
}

string get_time(){
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    stringstream ss2;
    if(timeinfo->tm_hour<10) ss2<<"0";
    ss2<<timeinfo->tm_hour<<":";
    if(timeinfo->tm_min<10) ss2<<"0";
    ss2<<timeinfo->tm_min<<":";
    if(timeinfo->tm_sec<10) ss2<<"0";
    ss2<<timeinfo->tm_sec;
    return ss2.str();
}

string get_time_date(){
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    stringstream ss2;
    if(timeinfo->tm_hour<10) ss2<<"0";
    ss2<<timeinfo->tm_hour<<":";
    if(timeinfo->tm_min<10) ss2<<"0";
    ss2<<timeinfo->tm_min<<":";
    if(timeinfo->tm_sec<10) ss2<<"0";
    ss2<<timeinfo->tm_sec<<", ";
    if(timeinfo->tm_mday<10) ss2<<"0";
    ss2<<timeinfo->tm_mday<<".";
    if(timeinfo->tm_mon+1<10) ss2<<"0";
    ss2<<timeinfo->tm_mon+1<<".";
    ss2<<timeinfo->tm_year+1900;
    return ss2.str();
}

bool count_time(string exec){
    cout<<"Start: "<<get_time()<<endl;
    double start_t=(double)clock();
    if(!system2(exec)){
        cout<<"Polecenie wykonane z bledem: "<<exec<<endl;
    }
    double sekundy=((double)clock()-start_t)/CLOCKS_PER_SEC;
    int minuty = ((int)sekundy)/60;
    sekundy -= minuty*60;
    cout<<"Czas wykonania: ";
    if(minuty>0) cout<<minuty<<" min ";
    cout<<sekundy<<" s"<<endl;
    return true;
}
