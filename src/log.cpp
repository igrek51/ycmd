#include "log.h"

int Log::log_level = INFO;
int Log::errors_count = 0;


void Log::error(string s) {
    echo("[ERROR] " + s, ERROR);
    errors_count++;
}

void Log::warn(string s) {
    echo("[warn] " + s, WARN);
}

void Log::info(string s) {
    echo(s, INFO);
}

void Log::debug(string s) {
    echo("[debug] " + s, DEBUG);
}

bool Log::isError() {
    return errors_count > 0;
}

void Log::echo(string s, int level) {
    if (level > log_level) return;
    if (level == ERROR) {
        cerr << s << endl;
    } else {
        cout << s << endl;
    }
}
