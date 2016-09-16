#include "system.h"
#include "args.h"
#include "path.h"

#include <ctime>
#include <sstream>
#include <termios.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>

bool system2(string cmd) {
    Log::debug("system :: " + cmd);
    if (system(cmd.c_str()) == 0)
        return true;
    Log::error("Blad polecenia: " + cmd);
    return false;
}

bool system_echo(string cmd) {
    Log::info("Wykonywanie: " + cmd);
    return system2(cmd);
}

bool set_env(string variable, string value) {
    if (setenv(variable.c_str(), value.c_str(), 1) != 0) {
        Log::error("Blad zmiany zmiennej srodowiskowej: " + variable);
        return false;
    }
    return true;
}

bool add_env_path(string new_path) {
    char* env_path = getenv("PATH");
    if (env_path == nullptr) {
        Log::error("Blad pobrania zmiennej srodowiskowej PATH");
        return false;
    }
    new_path = new_path + ":" + env_path;
    return set_env("PATH", new_path);
}

bool set_workdir(string wd) {
    wd = Path::reformat(wd);
    if (chdir(wd.c_str()) != 0) {
        Log::error("Blad zmiany katalogu roboczego: " + wd);
        return false;
    }
    return true;
}

string get_time() {
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    stringstream ss2;
    if (timeinfo->tm_hour < 10) ss2 << "0";
    ss2 << timeinfo->tm_hour << ":";
    if (timeinfo->tm_min < 10) ss2 << "0";
    ss2 << timeinfo->tm_min << ":";
    if (timeinfo->tm_sec < 10) ss2 << "0";
    ss2 << timeinfo->tm_sec;
    return ss2.str();
}

string get_time_date() {
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    stringstream ss2;
    if (timeinfo->tm_hour < 10) ss2 << "0";
    ss2 << timeinfo->tm_hour << ":";
    if (timeinfo->tm_min < 10) ss2 << "0";
    ss2 << timeinfo->tm_min << ":";
    if (timeinfo->tm_sec < 10) ss2 << "0";
    ss2 << timeinfo->tm_sec << ", ";
    if (timeinfo->tm_mday < 10) ss2 << "0";
    ss2 << timeinfo->tm_mday << ".";
    if (timeinfo->tm_mon + 1 < 10) ss2 << "0";
    ss2 << timeinfo->tm_mon + 1 << ".";
    ss2 << timeinfo->tm_year + 1900;
    return ss2.str();
}

bool count_time(string exec) {
    Log::info("Start: " + get_time());
    double start_t = (double) clock();
    if (!system2(exec)) {
        Log::error("Polecenie wykonane z bledem: " + exec);
    }
    double sekundy = ((double) clock() - start_t) / CLOCKS_PER_SEC;
    int minuty = ((int) sekundy) / 60;
    sekundy -= minuty * 60;
    stringstream ss;
    ss << "Czas wykonania: ";
    if (minuty > 0) ss << minuty << " min ";
    ss << sekundy << " s";
    Log::info(ss.str());
    return true;
}


char getch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = (char) getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}