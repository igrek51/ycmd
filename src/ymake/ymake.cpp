#include "ymake.h"
#include "../files.h"
#include "../version_increment.h"
#include "../args.h"
#include "../system.h"
#include "../path.h"

#include <fstream>
#include <sstream>

bool ymake_analyze_headers(YmakeDataSource* ds, bool& rebuild) {
    vector<string>* headers = ds->getHeaders();
    for (unsigned int i = 0; i < headers->size(); i++) {
        string file_header = Path::append(ds->src_path, headers->at(i));
        string file_header_prv = Path::append("prv", headers->at(i));
        if (!file_exists(file_header)) {
            Log::error("brak pliku: " + file_header);
            delete headers;
            return false;
        }
        if (files_equal(file_header, file_header_prv))
            continue;
        //przebudowa całego projektu
        Log::debug("Zmodyfikowany plik naglowkowy: " + file_header);
        rebuild = true;
        //skopiowanie nowej wersji z nadpisaniem
        if (!mkdir_overwrite_file(file_header, file_header_prv)) {
            Log::error("blad kopiowania do pliku " + file_header_prv);
            delete headers;
            return false;
        }
    }
    delete headers;
    return true;
}

bool ymake_analyze_srcs(YmakeDataSource* ds, bool& rebuild, vector<string>* objs) {
    vector<string>* srcs = ds->getSources();
    if (srcs->size() == 0) {
        delete srcs;
        return false;
    }
    //znalezienie zmienionych plików cpp
    for (unsigned int i = 0; i < srcs->size(); i++) {
        string file_src = Path::append(ds->src_path, srcs->at(i));
        string file_src_prv = Path::append("prv", srcs->at(i));
        string file_src_obj =
                Path::append("obj", Path::formatUnderscore(Path::removeExtenstion(srcs->at(i)))) +
                ".o";
        objs->push_back(file_src_obj);
        if (!file_exists(file_src)) {
            Log::error("brak pliku zrodlowego " + file_src);
            delete srcs;
            return false;
        }
        if (!rebuild) {
            if (file_exists(file_src_obj)) {
                if (files_equal(file_src, file_src_prv)) {
                    continue;
                }
            }
            rebuild = true;
        }
        //kompilacja zmienionych plików
        stringstream ss;
        ss << ds->compiler << " -c -o \"" << file_src_obj << "\" \"" << file_src << "\"";
        if (ds->compiler_flags.length() > 0) ss << " " << ds->compiler_flags;
        Log::info("Kompilacja " + file_src + ": " + ss.str());
        if (!system2(ss.str())) {
            Log::error("blad kompilacji pliku: " + file_src);
            delete srcs;
            return false;
        }
        //skopiowanie nowej wersji z nadpisaniem
        if (!mkdir_overwrite_file(file_src, file_src_prv)) {
            Log::error("blad kopiowania do pliku " + file_src_prv);
            delete srcs;
            return false;
        }
    }
    delete srcs;
    return true;
}

bool ymake_analyze_resources(YmakeDataSource* ds, bool& rebuild, vector<string>* objs) {
    if (ds->resource.length() > 0) {
        if (!file_exists(ds->resource)) {
            Log::error("brak pliku zasobow: " + ds->resource);
            return false;
        }
        //sprawdzenie starej wersji
        string resource_prv = Path::append("prv", ds->resource);
        string resource_obj =
                Path::append("obj", Path::formatUnderscore(Path::removeExtenstion(ds->resource))) +
                ".o";
        objs->push_back(resource_obj);
        if (!files_equal(ds->resource, resource_prv) || !file_exists(resource_obj)) {
            rebuild = true;
            Log::info("Dodawanie zasobow: " + ds->resource);
            //TOOD tworzenie zasobów w linuxie
            if (!system2("windres \"" + ds->resource + "\" \"" + resource_obj + "\""))
                return false;
            if (!mkdir_overwrite_file(ds->resource, resource_prv)) {
                Log::error("blad kopiowania do pliku " + resource_prv);
                return false;
            }
        }
    }
    return true;
}

bool ymake_linker_build(YmakeDataSource* ds, vector<string>* objs) {
    //konsolidacja całości aplikacji
    stringstream ss2;
    string bin_filename = Path::append("bin", ds->output);
    ss2 << ds->compiler << " -o \"" << bin_filename << "\"";
    for (unsigned int i = 0; i < objs->size(); i++) {
        ss2 << " \"" << objs->at(i) << "\"";
    }
    if (ds->libs.length() > 0) ss2 << " " << ds->libs;
    if (ds->linker_flags.length() > 0) ss2 << " " << ds->linker_flags;
    Log::info("Konsolidacja: " + ss2.str());
    if (!system2(ss2.str())) {
        Log::error("blad konsolidacji aplikacji");
        return false;
    }
    return true;
}

bool ymake_version_increment(YmakeDataSource* ds) {
    if (ds->version_file.length() > 0) {
        string version_file_1 = Path::append(ds->src_path, ds->version_file);
        if (!version_increment(version_file_1)) {
            Log::error("blad inkrementacji wersji w pliku: " + version_file_1);
            return false;
        }
    }
    return true;
}

bool ymake(string ymake_filename) {
    Log::info("ymake v" + IO::version + ":");

    YmakeDataSource* ds = new YmakeDataSource(ymake_filename);
    if (Log::isError() || !ds->validate()) return false;

    mkdir_if_n_exist("prv");
    mkdir_if_n_exist("obj");

    //znalezienie zmienionych plików nagłówkowych
    bool rebuild = false;
    if (!ymake_analyze_headers(ds, rebuild)) return false;
    if (rebuild) {
        Log::info("Przebudowywanie...");
    }

    vector<string>* objs = new vector<string>();
    if (!ymake_analyze_srcs(ds, rebuild, objs)) return false;
    if (!ymake_analyze_resources(ds, rebuild, objs)) return false;

    //budowanie
    mkdir_if_n_exist("bin");
    if (rebuild) {
        //inkrementacja wersji
        if (!ymake_version_increment(ds)) return false;
        //linkowanie
        if (!ymake_linker_build(ds, objs)) return false;
        Log::info("Zakonczono budowanie.");
    } else {
        Log::info("Brak zmian do wprowadzenia.");
    }

    delete objs;
    return true;
}

bool ymake_generate_bash(string ymake_filename, string output_filename) {
    output_filename = Path::reformat(output_filename);
    stringstream output;
    YmakeDataSource* ymake = new YmakeDataSource(ymake_filename);
    if (Log::isError() || !ymake->validate()) return false;

    //lista SRC
    vector<string>* srcs = ymake->getSources();
    if (srcs->size() == 0) return false;

    //utworzenie folderów
    output << "mkdir obj" << endl;
    output << "mkdir bin" << endl;
    vector<string>* objs = new vector<string>();
    //kompilacja plików cpp
    for (unsigned int i = 0; i < srcs->size(); i++) {
        string file_src = Path::append(ymake->src_path, srcs->at(i));
        string file_src_obj =
                Path::append("obj", Path::formatUnderscore(Path::removeExtenstion(srcs->at(i)))) +
                ".o";
        output << ymake->compiler << " -c -o \"" << file_src_obj << "\" \"" << file_src << "\"";
        if (ymake->compiler_flags.length() > 0) output << " " << ymake->compiler_flags;
        output << endl;
        objs->push_back(file_src_obj);
    }
    //Zasoby
    if (ymake->resource.length() > 0) {
        //TODO tworzenie zasobów w linuxie
        string resource_obj = Path::append("obj", Path::formatUnderscore(
                Path::removeExtenstion(ymake->resource))) + ".o";
        output << "windres \"" << ymake->resource << "\" \"" << resource_obj << "\"" << endl;
        objs->push_back(resource_obj);
    }
    //konsolidacja całości aplikacji
    string bin_filename = Path::append("bin", ymake->output);
    output << ymake->compiler << " -o \"" << bin_filename << "\"";
    for (unsigned int i = 0; i < objs->size(); i++) {
        output << " \"" << objs->at(i) << "\"";
    }
    delete srcs;
    delete objs;
    if (ymake->libs.length() > 0) output << " " << ymake->libs;
    if (ymake->linker_flags.length() > 0) output << " " << ymake->linker_flags;
    output << endl;
    //zapiasnie do pliku wyjściowego
    ofstream plik;
    plik.open(output_filename.c_str());
    if (!plik.good()) {
        plik.close();
        Log::error("Blad zapisywania do pliku: " + output_filename);
        return false;
    }
    plik << output.str();
    plik.close();
    Log::info("Wygenerowano plik: " + output_filename);

    return true;
}

bool ymake_generate_makefile(string ymake_filename, string output_filename) {
    output_filename = Path::reformat(output_filename);
    stringstream output;
    YmakeDataSource* ymake = new YmakeDataSource(ymake_filename);
    if (Log::isError() || !ymake->validate()) return false;

    //lista SRC
    vector<string>* srcs = ymake->getSources();
    if (srcs->size() == 0) return false;

    //plik Makefile
    output << "BIN = bin" << endl;
    //TODO testy
    output << "CC = g++" << endl;
    //kompilacja zasobów
    if (ymake->resource.length() > 0) {
        //TODO tworzenie zasobów w linuxie
        //string ymake_path_windres = Path::append(ymake->compiler_path, "windres.exe");
        //output << "WINDRES = " << ymake_path_windres << endl;
    }
    output << "CFLAGS = -c " << ymake->compiler_flags << endl;
    output << "LFLAGS = " << ymake->linker_flags << endl;
    output << "LIBS = " << ymake->libs << endl;
    output << "OUTPUT_NAME = " << ymake->output << endl;
    output << "OBJS =";
    for (unsigned int i = 0; i < srcs->size(); i++) {
        string file_src_obj =
                Path::append("obj", Path::formatUnderscore(Path::removeExtenstion(srcs->at(i)))) +
                ".o";
        output << " \"" << file_src_obj << "\"";
    }
    if (ymake->resource.length() > 0) {
        string resource_obj = Path::append("obj", Path::formatUnderscore(
                Path::removeExtenstion(ymake->resource))) + ".o";
        output << " \"" << resource_obj << "\"";
    }
    output << endl << endl << endl;
    //target all
    output << "all: $(OBJS)" << endl;
    output << "\t$(CC) $(OBJS) $(LIBS) $(LFLAGS) -o $(BIN)/$(OUTPUT_NAME)" << endl << endl;
    //target clean
    output << "clean:" << endl;
    output << "\tdel /Q obj\\*.o" << endl;
    output << "\tdel /Q $(BIN)\\$(OUTPUT_NAME)" << endl << endl << endl;
    //pojedyncze pliki .o
    for (unsigned int i = 0; i < srcs->size(); i++) {
        string file_src = Path::append(ymake->src_path, srcs->at(i));
        string file_src_obj = Path::append("obj", Path::removeExtenstion(srcs->at(i))) + ".o";
        output << file_src_obj << ": " << file_src << endl;
        output << "\t$(CC) $(CFLAGS) " << file_src << " -o \"" << file_src_obj << "\"" << endl <<
        endl;
    }
    if (ymake->resource.length() > 0) {
//        string resource_obj = Path::append("obj", Path::removeExtenstion(ymake->resource)) + ".o";
//        output << resource_obj << ": " << ymake->resource << endl;
//        output << "\t$(WINDRES) " << ymake->resource << " \"" << resource_obj << "\"" << endl <<
//        endl;
    }
    delete srcs;
    //zapisanie do pliku wyjściowego
    ofstream plik;
    plik.open(output_filename.c_str());
    if (!plik.good()) {
        plik.close();
        Log::error("Blad zapisywania do pliku: " + output_filename);
        return false;
    }
    plik << output.str();
    plik.close();
    Log::info("Wygenerowano plik: " + output_filename);

    return true;
}

bool run_from_ymake(string ymake_filename, int mode) {
    YmakeDataSource* ymake = new YmakeDataSource(ymake_filename);
    if (Log::isError() || !ymake->validate()) return false;
    //zmiana katalogu roboczego na ./bin
    if (!set_workdir(Path::reformat("./bin"))) {
        Log::error("blad zmiany katalogu roboczego na \"bin\"");
        return false;
    }
    stringstream ss;
    if (mode == 1) { // zwykły run - system
        ss << "\"" << ymake->output << "\"";
        Log::info("Uruchamianie: " + ss.str());
        return system2(ss.str());
    }
    return false;
}


bool clean_dir(string dir) {
    if (!dir_exists(dir)) {
        Log::debug("brak folderu " + dir);
    } else {
        vector<string>* files = get_files_from_dir(dir);
        for (unsigned int i = 0; i < files->size(); i++) {
            string file_del = Path::append(dir, files->at(i));
            Log::info("Usuwanie: " + file_del);
            if (!delete_file(file_del)) {
                Log::error("blad usuwania pliku " + file_del);
                return false;
            }
        }
        //usunięcie plików w podkatalogach
        vector<string>* dirs = get_dirs_from_dir(dir);
        for (unsigned int i = 0; i < dirs->size(); i++) {
            if (!clean_dir(Path::append(dir, dirs->at(i)))) {
                Log::error("blad czyszczenia podkatalogu " + dirs->at(i));
                return false;
            }
        }
    }
    return true;
}

bool clean_all() {
    Log::info("Czyszczenie...");
    if (!clean_dir("prv")) return false;
    if (!clean_dir("obj")) return false;
    Log::debug("Zakonczono czyszczenie...");
    return true;
}

bool ymake_init() {
    string filename = "ymake";
    //zapis do pliku
    fstream plik;
    plik.open(filename.c_str(), fstream::out | fstream::binary);
    if (!plik.good()) {
        Log::error("blad zapisu do pliku: " + filename);
        return false;
    }
    plik << "# -- " << get_time_date() << " -- generated by ymake v" << IO::version << " --" <<
    endl;
    plik << "COMPILER = g++" << endl << endl;
    plik << "#folder plików źródłowych i nagłówkowych (pusty - katalog roboczy)" << endl;
    plik << "SRC_PATH =" << endl << endl;
    plik <<
    "#rozszerzona lista plików źródłowych (oddzielonych spacją lub przecinkiem, *.cpp - dodaje wszystkie pliki, -plik - odejmuje plik)" <<
    endl;
    plik << "SRC = *.cpp" << endl << endl;
    plik <<
    "#rozszerzona lista dodatkowych plików projektu (przy wykrytej zmianie - przebudowanie projektu)" <<
    endl;
    plik << "HEADERS = *.h -version.h" << endl << endl;
    plik << "#nazwa aplikacji tworzona w folderze bin" << endl;
    plik << "OUTPUT = ycmd.exe" << endl << endl;
    plik << "#biblioteki dołączane jako flagi linkera" << endl;
    plik << "LIBS =" << endl << endl;
    plik << "#flagi kompilatora" << endl;
    plik << "COMPILER_FLAGS = -Wall" << endl << endl;
    plik << "#flagi linkera" << endl;
    plik << "LINKER_FLAGS = -Wall -s" << endl << endl;
    plik << "#plik zasobów dołączany do aplikacji" << endl;
    plik << "RESOURCE =" << endl << endl;
    plik << "#plik do inkrementacji wersji w SRC_PATH, przechowuje numer następnej wersji" << endl;
    plik << "VERSION_FILE = version.h" << endl << endl;
    plik.close();
    Log::info("Utworzono nowy plik ymake.");
    return true;
}
