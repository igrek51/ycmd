#include "ymake_data_source.h"
#include "variables.h"
#include "log.h"
#include "path.h"

YmakeDataSource::YmakeDataSource(string filename) {
    filename = Path::reformat(filename);
    vector<Variable *> *variables = get_variables(filename);
    if (variables == NULL) {
        Log::error("brak poprawnego pliku \"" + filename + "\"");
        return;
    }
    //odczytanie z pliku parametrów
    compiler = get_var_string(variables, "COMPILER");
    compiler_path = get_var_string(variables, "COMPILER_PATH");
    src = get_var_string(variables, "SRC");
    headers = get_var_string(variables, "HEADERS");
    src_path = get_var_string(variables, "SRC_PATH");
    output = get_var_string(variables, "OUTPUT");
    libs = get_var_string(variables, "LIBS");
    compiler_flags = get_var_string(variables, "COMPILER_FLAGS");
    linker_flags = get_var_string(variables, "LINKER_FLAGS");
    resource = get_var_string(variables, "RESOURCE");
    version_file = get_var_string(variables, "VERSION_FILE");

    for (unsigned int i = 0; i < variables->size(); i++) {
        delete variables->at(i);
    }
    delete variables;
}

bool YmakeDataSource::validate() {
    if (compiler.length() == 0) compiler = "g++";
    if (output.length() == 0) output = "main.exe";
    output = Path::reformat(output);
    if (compiler_path.length() == 0) {
        Log::error("Brak sciezki do kompilatora (COMPILER_PATH)");
        return false;
    }
    compiler_path = Path::reformat(compiler_path);
    if (compiler_path.compare(".") == 0) {
        compiler_path = "";
    }
    if (src.length() == 0) {
        Log::error("Brak plikow zrodlowych (SRC)");
        return false;
    }
    src_path = Path::reformat(src_path);
    if (src_path.compare(".") == 0) {
        src_path = "";
    }
    resource = Path::reformat(resource);
    version_file = Path::reformat(version_file);
    return true;
}

vector<string> *YmakeDataSource::getSources() {
    vector<string> *srcs = get_list_ex(src, src_path);
    if (srcs->size() == 0) {
        Log::error("Lista plikow zrodlowych jest pusta");
    }
    return srcs;
}

vector<string> *YmakeDataSource::getHeaders() {
    vector<string> *headersList = get_list_ex(headers, src_path);
    return headersList;
}
