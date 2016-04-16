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
    mkdir_if_n_exist("prv");
    mkdir_if_n_exist("obj");

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

    //Budowanie
    mkdir_if_n_exist("bin");
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
    output_filename = Path::reformat(output_filename);
    stringstream output;
    YmakeData* ymake = new YmakeData(ymake_filename);
    if(Log::isError() || !ymake->validate()) return false;

    //lista SRC
    vector<string>* srcs = ymake->getSources();
    if(srcs->size()==0){
        Log::error("Lista plikow zrodlowych jest pusta");
        return false;
    }

    //dodanie ścieżki kompilatora do zmiennej środowiskowej PATH
    output<<"set PATH=%PATH%;"<<ymake->compiler_path<<endl;
    //utworzenie folderów
    output<<"mkdir obj"<<endl;
    output<<"mkdir bin"<<endl;
    vector<string>* objs = new vector<string>();
    //kompilacja plików cpp
    for(unsigned int i=0; i<srcs->size(); i++){
        string file_src = Path::append(ymake->src_path, srcs->at(i));
        string file_src_obj = Path::append("obj", Path::removeExtenstion(srcs->at(i))) + ".o";
        output<<ymake->compiler<<" -c -o \""<<file_src_obj<<"\" "<<file_src;
        if(ymake->compiler_flags.length()>0) output<<" "<<ymake->compiler_flags;
        output<<endl;
        objs->push_back(file_src_obj);
    }
    //Zasoby
    if(ymake->resource.length()>0){
        string resource_obj = Path::append("obj", Path::removeExtenstion(ymake->resource)) + ".o";
        output<<"windres "<<ymake->resource<<" \""<<resource_obj<<"\""<<endl;
        objs->push_back(resource_obj);
    }
    //konsolidacja całości aplikacji
    string bin_filename = Path::append("bin", ymake->output);
    output<<ymake->compiler<<" -o \""<<bin_filename<<"\"";
    for(unsigned int i=0; i<objs->size(); i++){
        output<<" \""<<objs->at(i)<<"\"";
    }
    delete srcs;
    delete objs;
    if(ymake->libs.length()>0) output<<" "<<ymake->libs;
    if(ymake->linker_flags.length()>0) output<<" "<<ymake->linker_flags;
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

    return true;
}

bool ymake_generate_makefile(string ymake_filename, string output_filename){
    output_filename = Path::reformat(output_filename);
    stringstream output;
    YmakeData* ymake = new YmakeData(ymake_filename);
    if(Log::isError() || !ymake->validate()) return false;

    //lista SRC
    vector<string>* srcs = ymake->getSources();
    if(srcs->size()==0){
        Log::error("Lista plikow zrodlowych jest pusta");
        return false;
    }

    //plik Makefile
    string ymake_path_cc = Path::append(ymake->compiler_path, "mingw32-g++.exe");
    output<<"BIN = bin"<<endl;
    output<<"CC = "<<ymake_path_cc<<endl;
    //kompilacja zasobów
    if(ymake->resource.length()>0){
        string ymake_path_windres = Path::append(ymake->compiler_path, "windres.exe");
        output<<"WINDRES = "<<ymake_path_windres<<endl;
    }
    output<<"CFLAGS = -c "<<ymake->compiler_flags<<endl;
    output<<"LFLAGS = "<<ymake->linker_flags<<endl;
    output<<"LIBS = "<<ymake->libs<<endl;
    output<<"OUTPUT_NAME = "<<ymake->output<<endl;
    output<<"OBJS =";
    for(unsigned int i=0; i<srcs->size(); i++){
        string file_src_obj = Path::append("obj", Path::removeExtenstion(srcs->at(i))) + ".o";
        output<<" "<<file_src_obj;
    }
    if(ymake->resource.length()>0){
        string resource_obj = Path::append("obj", Path::removeExtenstion(ymake->resource)) + ".o";
        output<<" "<<resource_obj;
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
        string file_src = Path::append(ymake->src_path, srcs->at(i));
        string file_src_obj = Path::append("obj", Path::removeExtenstion(srcs->at(i))) + ".o";
        output<<file_src_obj<<": "<<file_src<<endl;
        output<<"\t$(CC) $(CFLAGS) "<<file_src<<" -o \""<<file_src_obj<<"\""<<endl<<endl;
    }
    if(ymake->resource.length()>0){
        string resource_obj = Path::append("obj", Path::removeExtenstion(ymake->resource)) + ".o";
        output<<resource_obj<<": "<<ymake->resource<<endl;
        output<<"\t$(WINDRES) "<<ymake->resource<<" \""<<resource_obj<<"\""<<endl<<endl;
    }
    delete srcs;
    //zapisanie do pliku wyjściowego
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
