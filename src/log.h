#ifndef LOG_H
#define LOG_H

#include <iostream>

using namespace std;

class Log {
public:
    /**
       @brief log_level Poziom wyświetlania logów
       0 - wyłączony
       1 - ERROR
       2 - WARN (+ ERROR)
       3 - INFO (+ WARN + ERROR)
       4 - DEBUG (wszystkie)
     */
    static int log_level;
    static int errors_count;

    static void error(string s);
    static void warn(string s);
    static void info(string s);
    static void debug(string s);
private:
    static void echo(string s, int level);
};

#endif
