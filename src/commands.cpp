#include "commands.h"
#include "files.h"
#include "args.h"
#include "system.h"
#include "version_increment.h"
#include "ymake/ymake.h"
#include "input.h"
#include "path.h"

#include <sstream>

bool exec_commands(vector<string> *args) {
    string next1;
    string next2;
    int next;
    //log level
    if (is_arg("--log=debug", args) || is_arg("--debug", args)) {
        Log::log_level = Log::DEBUG;
    } else if (is_arg("--log=info", args) || is_arg("--info", args)) {
        Log::log_level = Log::INFO;
    } else if (is_arg("--log=warn", args) || is_arg("--warn", args)) {
        Log::log_level = Log::WARN;
    } else if (is_arg("--log=error", args) || is_arg("--error", args)) {
        Log::log_level = Log::ERROR;
    } else if (is_arg("--log=off", args)) {
        Log::log_level = Log::OFF;
    }
    //zmiana katalogu roboczego
    if (next_arg("-w", args, next1) || next_arg("--workdir", args, next1)) {
        if (!set_workdir(next1)) {
            Log::error("nieprawidlowa sciezka \"" + next1 + "\"");
            return false;
        }
    }
    if (next_arg("--setenv", args, next1, next2)) { //zmienna środowiskowa
        return set_env(next1, next2);
    }
    if (next_arg("--setpath", args, next1)) { //zmienna środowiskowa PATH
        return set_env("PATH", next1);
    }
    if (next_arg("--addpath", args, next1)) { //zmienna środowiskowa PATH
        if (!add_env_path(next1)) {
            Log::error("blad dodawania do zmiennej srodowiskowej PATH");
            return false;
        }
    }
    //POLECENIA
    if (next_arg_number("-e", args, next) ||
        next_arg_number("--exec", args, next)) { //wykonaj polecenia z parametrów
        for (unsigned int i = (unsigned int) next; i < args->size(); i++) {
            if (!system_echo(args->at(i)))
                return false; //przerwij łancuch w przypadku błędu
        }
    } else if (next_arg_number("-1", args, next) ||
               next_arg_number("--e1", args, next)) { //wykonaj jako 1 polecenie
        //złożenie argumentów w jedno polecenie
        stringstream ss;
        for (unsigned int i = (unsigned int) next; i < args->size(); i++) {
            ss << args->at(i);
            if (i < args->size() - 1) ss << " ";
        }
        return system_echo(ss.str());
    } else if (next_arg("-f", args, next1) ||
               next_arg("--file", args, next1)) { //wykonaj wiersze z pliku
        Log::info("Wykonywanie zawartosci pliku \"" + next1 + "\":");
        vector<string> *linie = get_nonempty_lines(next1);
        if (linie == NULL) {
            Log::error("brak pliku \"" + next1 + "\"");
            delete linie;
            return false;
        }
        for (unsigned int i = 0; i < linie->size(); i++) {
            if (!system_echo(linie->at(i))) { //przerwij łancuch w przypadku błędu
                delete linie;
                return false;
            }
        }
        delete linie;
    } else if (next_arg("-t", args, next1) ||
               next_arg("--time", args, next1)) { //zmierz czas wykonania
        return count_time(next1);
    } else if (next_arg("-y", args, next1, "ymake") ||
               next_arg("--ymake", args, next1, "ymake")) { //skompiluj
        if (!ymake(next1))
            return false;
    } else if (next_arg2("--ymake-bat", args, next1, next2, "ymake",
                         "build.bat")) { //generuj plik .bat
        if (!ymake_generate_bat(next1, next2))
            return false;
    } else if (next_arg2("--ymake-makefile", args, next1, next2, "ymake",
                         "Makefile")) { //generuj plik makefile
        if (!ymake_generate_makefile(next1, next2))
            return false;
    } else if (next_arg("--run", args, next1, "ymake")) { //uruchom aplikację z ymake
        if (!run_from_ymake(next1, 1)) {
            Log::error("blad uruchamiania z pliku: " + next1);
            return false;
        }
    } else if (is_arg("--init-ymake", args)) {
        return ymake_init();
    } else if (is_arg("--init-version", args)) {
        return version_init();
    } else if (is_arg("--init", args)) {
        bool wynik1 = ymake_init();
        bool wynik2 = version_init();
        return wynik1 && wynik2;
    } else if (is_arg("-i", args) || is_arg("--input", args)) { //czekaj na wpisanie z klawiatury
        return input_cmd();
    } else if (is_arg("--clean", args)) { //wyczysc folder prv
        return clean_all();
    } else if (next_arg("--version++", args, next1, "version.h")) { //zwiększ wersję w pliku
        return version_increment(Path::reformat(next1));
    } else if (is_arg("--test", args)) {

    } else {
        Log::error("brak poprawnego polecenia");
        return false;
    }
    return true;
}

void show_help() {
    cout << "ycmd v " << IO::version << endl << endl;
    cout << "Polecenia:" << endl;
    cout << "ycmd -e(--exec) [polecenie1] [polecenie2] [...] - wykonaj polecenia" << endl;
    cout << "ycmd -1(--e1) [po le ce nie] - wykonaj jako jedno polecenie" << endl;
    cout << "ycmd -f(--file) [plik] - wykonaj wiersze z pliku (przerwij w przypadku bledu)" << endl;
    cout << "ycmd -y(--ymake) [ymake] - zbuduj na podstawie pliku ymake" << endl;
    cout << "ycmd --ymake-bat [ymake] [build.bat] - utworz plik .bat na podstawie pliku ymake" <<
    endl;
    cout <<
    "ycmd --ymake-makefile [ymake] [Makefile] - utworz plik Makefile na podstawie pliku ymake" <<
    endl;
    cout << "ycmd -t(--time) [polecenie] - zmierz czas wykonania polecenia" << endl;
    cout << "ycmd -i(--input) - czekaj na wpisanie polecenia" << endl;
    cout << "ycmd --clean - wyczysc folder prv i obj" << endl;
    cout << "ycmd --run [ymake] - uruchom z pliku ymake" << endl;
    cout << "ycmd --version++ [version.h] - zwieksz numer wersji w pliku" << endl;
    cout << "ycmd --init - utworz plik ymake oraz plik wersji version.h" << endl;
    cout << "ycmd --init-ymake - utworz plik ymake" << endl;
    cout << "ycmd --init-version - utworz plik wersji version.h" << endl;
    cout << endl;
    cout << "Opcjonalne parametry:" << endl;
    cout << "-w(--workdir) [dir] - zmien katalog roboczy" << endl;
    cout << "--setenv [var] [value] - ustaw zmienna srodowiskowa" << endl;
    cout << "--setpath [path] - ustaw zmienna srodowiskowa PATH" << endl;
    cout << "--addpath [path] - dodaj do zmiennej srodowiskowej PATH" << endl;
    cout << "--log=[debug|info|warn|error|off] (--debug, --info, --warn, --error) - poziom wyswietlania komunikatow" << endl;
    cout << "-p - czekaj na wcisniecie klawisza po zakonczeniu" << endl;
    cout << "--pe - czekaj na wcisniecie w przypadku bledu" << endl;
}
