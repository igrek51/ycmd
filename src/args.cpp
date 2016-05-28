#include "args.h"
#include "version.h"

bool IO::pause = false;
string IO::version = VERSION;

vector<string> *get_args(int argc, char **argv) {
    vector<string> *args = new vector<string>;
    for (int i = 1; i < argc; i++) {
        args->push_back(argv[i]);
    }
    return args;
}

int find_arg(string szukany, vector<string> *args) {
    for (unsigned int i = 0; i < args->size(); i++) {
        if (args->at(i) == szukany) return i;
    }
    return -1;
}

bool is_arg(string szukany, vector<string> *args) {
    return find_arg(szukany, args) != -1;
}

bool next_arg_number(string szukany, vector<string> *args, int &next) {
    int i = find_arg(szukany, args);
    if (i == -1) return false; //nie znalazło szukanego argumentu
    if (i + 1 < (int) args->size()) { //jeśli istnieje następny element
        next = i + 1;
        return true;
    } else {
        Log::error("Brak parametru po: " + szukany);
        return false; //nie istnieje następny element
    }
}

bool next_arg(string szukany, vector<string> *args, string &next_text, string domyslny) {
    int i = find_arg(szukany, args);
    if (i == -1) return false; //nie znalazło szukanego argumentu
    if (i + 1 < (int) args->size()) { //jeśli istnieje następny element
        next_text = args->at((unsigned int) (i + 1));
        return true;
    } else { //nie istnieje następny element
        if (domyslny.length() > 0) { //została podana wartość domyślna
            next_text = domyslny;
            Log::debug(
                    "Przyjeto domyslna wartosc 1. parametru po \"" + szukany + "\": " + domyslny);
            return true;
        } else { //brak zdefiniowanej wartości domyślnej
            Log::error("Brak parametru po: " + szukany);
            return false;
        }
    }
}

bool next_arg2(string szukany, vector<string> *args, string &next_text1, string &next_text2,
               string domyslny1, string domyslny2) {
    int i = find_arg(szukany, args);
    if (i == -1) return false; //nie znalazło szukanego argumentu
    if (i + 2 < (int) args->size()) { //jeśli istnieją 2 następne elementy
        next_text1 = args->at((unsigned int) (i + 1));
        next_text2 = args->at((unsigned int) (i + 2));
        return true;
    } else if (i + 1 < (int) args->size()) { //istnieje 1 następny element
        if (domyslny2.length() > 0) { //została podana 2. wartość domyślna
            next_text1 = args->at((unsigned int) (i + 1));
            next_text2 = domyslny2;
            Log::debug(
                    "Przyjeto domyslna wartosc 2. parametru po \"" + szukany + "\": " + domyslny2);
            return true;
        } else { //brak zdefiniowanych wartości domyślnych
            Log::error("Brak drugiego parametru po " + szukany);
            return false; //nie istnieją oba
        }
    } else { //nie istnieje żodyn następny
        if (domyslny1.length() > 0 && domyslny2.length() > 0) { //podane obie wartości domyślne
            next_text1 = domyslny1;
            next_text2 = domyslny2;
            Log::debug(
                    "Przyjeto domyslna wartosc 1. parametru po \"" + szukany + "\": " + domyslny1);
            Log::debug(
                    "Przyjeto domyslna wartosc 2. parametru po \"" + szukany + "\": " + domyslny2);
            return true;
        } else { //brak zdefiniowanych wartości domyślnych
            Log::error("Brak 2 parametrow po " + szukany);
            return false; //nie istnieją oba
        }
    }
}
