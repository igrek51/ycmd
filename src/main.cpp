#include "commands.h"
#include "args.h"

#include <conio.h>

using namespace std;

int main(int argc, char **argv) {
    //pobranie argumentów do kontenera
    vector<string> *args = get_args(argc, argv);
    //pomoc (brak parametru)
    if (is_arg("--help", args) || is_arg("-h", args) || args->size() == 0) {
        show_help();
        return 0;
    }
    //flaga pause
    if (is_arg("-p", args)) IO::pause = true;
    //wykonywanie polecenia
    if (!exec_commands(args))
        Log::errors_count++;
    //pause if error
    if (Log::errors_count > 0 && (is_arg("--pe", args) || is_arg("-pe", args))) IO::pause = true;
    if (IO::pause) {
        if (Log::errors_count > 0) {
            Log::info("Program zakonczony bledem...");
        } else {
            Log::info("Pomyslnie zakonczono program...");
        }
        getch();
    } else {
        Log::debug("Zakonczono.");
    }
    delete args;
    if (Log::errors_count > 0) return 1;
    return 0;
}
