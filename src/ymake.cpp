#include "ymake.h"
#include "files.h"
#include "versioning.h"
#include "variables.h"
#include "io.h"
#include "system.h"
#include "string_utils.h"
#include "ymake_data.h"
#include "path.h"

#include <fstream>
#include <sstream>
#include <windows.h>

bool ymake(string ymake_filename){
    Log::info("ymake v"+IO::version+":");

    YmakeData* ymake = new YmakeData(ymake_filename);
    if(Log::isError() || !ymake->validate()) return false;

    //lista SRC
    vector<string>* srcs = ymake->getSources();
    if(srcs->size()==0){
        Log::error("Lista plikow zrodlowych jest pusta");
        return false;
    }
    //lista plików nagłówkowych
    vector<string>* headers = ymake->getHeaders();

    //dodanie ścieżki kompilatora do zmiennej środowiskowej PATH
    add_env_path(ymake->compiler_path);

    //utworzenie folderów
    mkdir_if_n_exist("obj");
    mkdir_if_n_exist("prv");
    mkdir_if_n_exist("bin");

    //znalezienie zmienionych plików nagłówkowych
    bool rebuild = false, change = false;
    for(unsigned int i=0; i<headers->size(); i++){
        string file_header = Path::append(ymake->src_path, headers->at(i));
        string file_header_prv = Path::append("prv", headers->at(i));
        if(!file_exists(file_header)){
            Log::error("brak pliku: "+file_header);
            return false;
        }
        if(files_equal(file_header, file_header_prv))
            continue;
        //przebudowa całego projektu
        Log::info("Zmodyfikowany plik naglowkowy: "+file_header);
        rebuild = true;
        change = true;
        //skopiowanie nowej wersji z nadpisaniem
        if(!copy_files(file_header, file_header_prv)){
            Log::error("blad kopiowania do pliku " + file_header_prv);
            return false;
        }
    }
    delete headers;

    if(rebuild){
        Log::info("Przebudowywanie...");
    }

    vector<string>* objs = new vector<string>();

    //znalezienie zmienionych plików cpp
    for(unsigned int i=0; i<srcs->size(); i++){
        string file_src = Path::append(ymake->src_path, srcs->at(i));
        string file_src_prv = Path::append("prv", srcs->at(i));
        if(!file_exists(file_src)){
            Log::error("brak pliku zrodlowego "+file_src);
            return false;
        }
        string file_src_obj = Path::append("obj", Path::removeExtenstion(srcs->at(i))) + ".o";
        if(!rebuild){
            if(file_exists(file_src_obj)){
                if(files_equal(file_src, file_src_prv)){
                    continue;
                }
            }
            change = true;
        }
        //kompilacja zmienionych plików
        stringstream ss;
        ss<<ymake->compiler<<" -c -o \""<<file_src_obj<<"\" "<<file_src;
        if(ymake->compiler_flags.length()>0) ss<<" "<<ymake->compiler_flags;
        Log::info("Kompilacja "+file_src+": "+ss.str());
        if(!system2(ss.str())){
            Log::error("blad kompilacji pliku: "+file_src);
            return false;
        }
        objs->push_back(file_src_obj);
        //skopiowanie nowej wersji z nadpisaniem
        if(!copy_files(file_src, file_src_prv)){
            Log::error("blad kopiowania do pliku " + file_src_prv);
            return false;
        }
    }
    //Zasoby
    if(ymake->resource.length()>0){
        if(!file_exists(ymake->resource)){
            Log::error("brak pliku zasobow: " + ymake->resource);
            return false;
        }
        //sprawdzenie starej wersji
        string resource_prv = Path::append("prv", ymake->resource);
        string resource_obj = Path::append("obj", Path::removeExtenstion(ymake->resource)) + ".o";
        if(!files_equal(ymake->resource, resource_prv) || !file_exists(resource_obj)){
            change = true;
            Log::info("Dodawanie zasobow: "+ymake->resource);
            if(!system2("windres "+ymake->resource+" \""+resource_obj+"\""))
                return false;
            objs->push_back(resource_obj);
            if(!copy_files(ymake->resource, resource_prv)){
                Log::error("blad kopiowania do pliku " + resource_prv);
                return false;
            }
        }
    }
    if(change || rebuild){
        //inkrementacja wersji
        if(ymake->version_file.length()>0){
            string version_file_1 = Path::append(ymake->src_path, ymake->version_file);
            if(!version_inc(version_file_1)){
                Log::error("blad inkrementacji wersji w pliku: "+version_file_1);
                return false;
            }
        }

        //konsolidacja całości aplikacji
        stringstream ss2;
        string bin_filename = Path::append("bin", ymake->output);
        ss2<<ymake->compiler<<" -o \""<<bin_filename<<"\"";
        for(unsigned int i=0; i<objs->size(); i++){
            ss2<<" \""<<objs->at(i)<<"\"";
        }

        if(ymake->libs.length()>0) ss2<<" "<<ymake->libs;
        if(ymake->linker_flags.length()>0) ss2<<" "<<ymake->linker_flags;
        Log::info("Konsolidacja: "+ss2.str());
        if(!system2(ss2.str())){
            Log::error("blad konsolidacji aplikacji");
            return false;
        }
    }else{
        Log::info("Brak zmian do wprowadzenia.");
    }
    delete srcs;
    delete objs;
    return true;
}

bool ymake_generate_bat(string ymake_filename, string output_filename){
    /*
    ymake_filename = Path::reformat(ymake_filename);
    output_filename = Path::reformat(output_filename);
    stringstream output;
    vector<Variable*>* variables = get_variables(ymake_filename);
    if(variables==NULL){
        Log::error("brak poprawnego pliku \""+ymake_filename+"\"");
        return false;
    }
    //odczytanie z pliku parametrów
    string ymake_compiler = get_var_string(variables, "COMPILER");
    string ymake_compiler_path = get_var_string(variables, "COMPILER_PATH");
    string ymake_src = get_var_string(variables, "SRC");
    string ymake_src_path = get_var_string(variables, "SRC_PATH");
    string ymake_output = get_var_string(variables, "OUTPUT");
    string ymake_libs = get_var_string(variables, "LIBS");
    string ymake_compiler_flags = get_var_string(variables, "COMPILER_FLAGS");
    string ymake_linker_flags = get_var_string(variables, "LINKER_FLAGS");
    string ymake_resource = get_var_string(variables, "RESOURCE");
    for(unsigned int i=0; i<variables->size(); i++){
        delete variables->at(i);
    }
    delete variables;
    //walidacja wczytanych danych, wartości domyślne
    if(ymake_compiler.length()==0) ymake_compiler = "g++";
    if(ymake_output.length()==0) ymake_output = "main.exe";
    if(ymake_compiler_path.length()==0){
        Log::error("Brak sciezki do kompilatora (COMPILER_PATH)");
        return false;
    }
    if(ymake_src.length()==0){
        Log::error("Brak plikow zrodlowych (SRC)");
        return false;
    }
    //format ymake_src_path
    if(ymake_src_path.length()>0){
        if(ymake_src_path[ymake_src_path.length()-1]!='\\') ymake_src_path+='\\';
        if(ymake_src_path=="\\"||ymake_src_path==".\\") ymake_src_path = "";
    }
    //lista SRC
    vector<string> *srcs = get_list_ex(ymake_src, ymake_src_path);
    //dodanie ścieżki kompilatora do zmiennej środowiskowej PATH
    output<<"set PATH=%PATH%;"<<ymake_compiler_path<<endl;
    //utworzenie folderów
    output<<"mkdir obj"<<endl;
    output<<"mkdir bin"<<endl;
    //kompilacja plików cpp
    for(unsigned int i=0; i<srcs->size(); i++){
        //kompilacja plików cpp
        output<<ymake_compiler<<" -c -o \"obj\\"<<remove_extension(srcs->at(i))<<".o\" "<<ymake_src_path<<srcs->at(i);
        if(ymake_compiler_flags.length()>0) output<<" "<<ymake_compiler_flags;
        output<<endl;
    }
    //Zasoby
    if(ymake_resource.length()>0){
        output<<"windres "<<ymake_resource<<" \"obj\\"<<remove_extension(ymake_resource)<<".o\""<<endl;
    }
    //konsolidacja całości aplikacji
    output<<ymake_compiler<<" -o \"bin\\"<<ymake_output<<"\"";
    for(unsigned int i=0; i<srcs->size(); i++){
        output<<" \"obj\\"<<remove_extension(srcs->at(i))<<".o\"";
    }
    delete srcs;
    if(ymake_resource.length()>0) output<<" \"obj\\"<<remove_extension(ymake_resource)<<".o\"";
    if(ymake_libs.length()>0) output<<" "<<ymake_libs;
    if(ymake_linker_flags.length()>0) output<<" "<<ymake_linker_flags;
    output<<endl;
    //zapiasnie do pliku wyjściowego
    ofstream plik;
    plik.open(output_filename.c_str());
    if(!plik.good()){
        plik.close();
        Log::error("Blad zapisywania do pliku: "+output_filename);
        return false;
    }
    plik<<output.str();
    plik.close();
    Log::info("Wygenerowano plik: "+output_filename);
    */
    return true;
}

bool ymake_generate_makefile(string ymake_filename, string output_filename){
    /*
    ymake_filename = Path::reformat(ymake_filename);
    output_filename = Path::reformat(output_filename);
    vector<Variable*>* variables = get_variables(ymake_filename);
    if(variables==NULL){
        Log::error("brak poprawnego pliku \""+ymake_filename+"\"");
        return false;
    }
    //odczytanie z pliku parametrów
    string ymake_compiler = get_var_string(variables, "COMPILER");
    string ymake_compiler_path = get_var_string(variables, "COMPILER_PATH");
    string ymake_src = get_var_string(variables, "SRC");
    string ymake_src_path = get_var_string(variables, "SRC_PATH");
    string ymake_output = get_var_string(variables, "OUTPUT");
    string ymake_libs = get_var_string(variables, "LIBS");
    string ymake_compiler_flags = get_var_string(variables, "COMPILER_FLAGS");
    string ymake_linker_flags = get_var_string(variables, "LINKER_FLAGS");
    string ymake_resource = get_var_string(variables, "RESOURCE");
    for(unsigned int i=0; i<variables->size(); i++){
        delete variables->at(i);
    }
    delete variables;
    //walidacja wczytanych danych, wartości domyślne
    if(ymake_compiler.length()==0) ymake_compiler = "g++";
    if(ymake_output.length()==0) ymake_output = "main.exe";
    if(ymake_compiler_path.length()==0){
        Log::error("Brak sciezki do kompilatora (COMPILER_PATH)");
        return false;
    }
    if(ymake_src.length()==0){
        Log::error("Brak plikow zrodlowych (SRC)");
        return false;
    }
    //format ymake_src_path
    if(ymake_src_path.length()>0){
        if(ymake_src_path[ymake_src_path.length()-1]!='\\') ymake_src_path+='\\';
        if(ymake_src_path=="\\"||ymake_src_path==".\\") ymake_src_path = "";
    }
    //lista SRC
    vector<string> *srcs = get_list_ex(ymake_src, ymake_src_path);
    //plik Makefile
    stringstream output;
    output<<"BIN = bin"<<endl;
    if(ymake_compiler_path.length()==0) ymake_compiler_path = ".";
    if(ymake_compiler_path[ymake_compiler_path.length()-1]!='\\') ymake_compiler_path+="\\";
    output<<"CC = "<<ymake_compiler_path<<"mingw32-g++.exe"<<endl;
    //kompilacja zasobów
    if(ymake_resource.length()>0){
        output<<"WINDRES = "<<ymake_compiler_path<<"windres.exe"<<endl;
    }
    output<<"CFLAGS = -c "<<ymake_compiler_flags<<endl;
    output<<"LFLAGS = "<<ymake_linker_flags<<endl;
    output<<"LIBS = "<<ymake_libs<<endl;
    output<<"OUTPUT_NAME = "<<ymake_output<<endl;
    output<<"OBJS =";
    for(unsigned int i=0; i<srcs->size(); i++){
        output<<" obj/"<<remove_extension(srcs->at(i))<<".o";
    }
    if(ymake_resource.length()>0){
        output<<" obj/"<<remove_extension(ymake_resource)<<".o";
    }
    output<<endl<<endl<<endl;
    //target all
    output<<"all: $(OBJS)"<<endl;
    output<<"\t$(CC) $(OBJS) $(LIBS) $(LFLAGS) -o $(BIN)/$(OUTPUT_NAME)"<<endl<<endl;
    //target clean
    output<<"clean:"<<endl;
    output<<"\tdel /Q obj\\*.o"<<endl;
    output<<"\tdel /Q $(BIN)\\$(OUTPUT_NAME)"<<endl<<endl<<endl;
    //pojedyncze pliki .o
    for(unsigned int i=0; i<srcs->size(); i++){
        output<<"obj/"<<remove_extension(srcs->at(i))<<".o: "<<ymake_src_path<<srcs->at(i)<<endl;
        output<<"\t$(CC) $(CFLAGS) "<<ymake_src_path<<srcs->at(i)<<" -o obj/"<<remove_extension(srcs->at(i))<<".o"<<endl<<endl;
    }
    delete srcs;
    if(ymake_resource.length()>0){
        output<<"obj/"<<remove_extension(ymake_resource)<<".o: "<<ymake_resource<<endl;
        output<<"\t$(WINDRES) resource.rc \"obj/"<<remove_extension(ymake_resource)<<".o\""<<endl<<endl;
    }
    //zapiasnie do pliku wyjściowego
    ofstream plik;
    plik.open(output_filename.c_str());
    if(!plik.good()){
        plik.close();
        Log::error("Blad zapisywania do pliku: "+output_filename);
        return false;
    }
    plik<<output.str();
    plik.close();
    Log::info("Wygenerowano plik: "+output_filename);
    */
    return true;
}

bool run_from_ymake(string ymake_filename, int mode){
    YmakeData* ymake = new YmakeData(ymake_filename);
    if(Log::isError() || !ymake->validate()) return false;

    //zmiana katalogu roboczego na ./bin
    if(!set_workdir(Path::reformat("./bin"))){
        Log::error("blad zmiany katalogu roboczego na \"bin\"");
        return false;
    }
    stringstream ss;
    if(mode==1){ // zwykły run - system
        ss<<"\""<<ymake->output<<"\"";
        Log::info("Uruchamianie: "+ss.str());
        return system2(ss.str());
    }else if(mode==2){ //run start
        ss<<"start \"\" \""<<ymake->output<<"\"";
        Log::info("Uruchamianie: "+ss.str());
        return system2(ss.str());
    }else if(mode==3){ //shelexecute
        if((int)ShellExecute(0, "open", ymake->output.c_str(), "", 0, SW_SHOW) > 32){
            Log::info("Uruchomiono \""+ymake->output+"\"...");
            return true;
        }
        Log::error("Blad uruchamiania poleceniem ShellExecute: \""+ymake->output+"\"");
        return false;
    }
    return false;
}


bool clean_dir(string dir){
    if(!dir_exists(dir)){
        Log::debug("brak folderu " + dir);
    }else{
        vector<string>* files = get_files_from_dir(dir);
        for(unsigned int i=0; i<files->size(); i++){
            string file_del = Path::append(dir, files->at(i));
            Log::info("Usuwanie: " + file_del);
            if(!delete_file(file_del)){
                Log::error("blad usuwania pliku " + file_del);
                return false;
            }
        }
    }
    return true;
}

bool clean_all(){
    Log::info("Czyszczenie...");
    if(!clean_dir("prv")) return false;
    if(!clean_dir("obj")) return false;
    return true;
}

bool init_ymake(){
    string filename = "ymake";
    //zapis do pliku
    fstream plik;
    plik.open(filename.c_str(),fstream::out|fstream::binary);
    if(!plik.good()){
        Log::error("blad zapisu do pliku: "+filename);
        return false;
    }
    plik<<"// -- "<<get_time_date()<<" -- generated by ymake v"<<IO::version<<" --"<<endl;
    plik<<"COMPILER = g++"<<endl<<endl;
    plik<<"COMPILER_PATH = C:\\MinGW\\bin"<<endl<<endl;
    plik<<"SRC_PATH ="<<endl<<endl;
    plik<<"SRC = *.cpp"<<endl<<endl;
    plik<<"HEADERS = *.h -version.h"<<endl<<endl;
    plik<<"OUTPUT = ycmd.exe"<<endl<<endl;
    plik<<"LIBS ="<<endl<<endl;
    plik<<"COMPILER_FLAGS = -Wall"<<endl<<endl;
    plik<<"LINKER_FLAGS = -Wall -s"<<endl<<endl;
    plik<<"RESOURCE ="<<endl<<endl;
    plik<<"VERSION_FILE = version.h"<<endl<<endl;
    plik.close();
    Log::info("Utworzono nowy plik ymake");
    return true;
}
