#include "ymake.h"
#include "files.h"
#include "versioning.h"
#include "variables.h"
#include "io.h"
#include "system.h"

#include <fstream>
#include <sstream>
#include <windows.h>

bool ymake(string ymake_filename){
    ymake_filename = dir_format(ymake_filename);
    IO::echo("ymake v"+IO::version+":");
    vector<Variable*>* variables = get_variables(ymake_filename);
    if(variables==NULL){
        IO::error("brak poprawnego pliku \""+ymake_filename+"\"");
        return false;
    }
    //odczytanie z pliku parametrów
    string ymake_compiler = get_var_string(variables, "COMPILER");
    string ymake_compiler_path = get_var_string(variables, "COMPILER_PATH");
    string ymake_src = get_var_string(variables, "SRC");
    string ymake_headers = get_var_string(variables, "HEADERS");
    string ymake_src_path = get_var_string(variables, "SRC_PATH");
    string ymake_output = get_var_string(variables, "OUTPUT");
    string ymake_libs = get_var_string(variables, "LIBS");
    string ymake_compiler_flags = get_var_string(variables, "COMPILER_FLAGS");
    string ymake_linker_flags = get_var_string(variables, "LINKER_FLAGS");
    string ymake_resource = get_var_string(variables, "RESOURCE");
    string ymake_version_file = get_var_string(variables, "VERSION_FILE");
    for(unsigned int i=0; i<variables->size(); i++){
        delete variables->at(i);
    }
    delete variables;
    //walidacja wczytanych danych, wartoœci domyœlne
    if(ymake_compiler.length()==0) ymake_compiler = "g++";
    if(ymake_output.length()==0) ymake_output = "main.exe";
    if(ymake_compiler_path.length()==0){
        IO::error("Brak sciezki do kompilatora (COMPILER_PATH)");
        return false;
    }
    if(ymake_src.length()==0){
        IO::error("Brak plikow zrodlowych (SRC)");
        return false;
    }
    //format ymake_src_path
    if(ymake_src_path.length()>0){
        if(ymake_src_path[ymake_src_path.length()-1]!='\\') ymake_src_path+='\\';
        if(ymake_src_path=="\\"||ymake_src_path==".\\") ymake_src_path = "";
    }
    //lista SRC
    vector<string> *srcs = get_list_ex(ymake_src, ymake_src_path);
    if(srcs->size()==0){
        IO::error("pusta lista plikow zrodlowych");
        return false;
    }
    //lista plików headers (dodatkowych plików projektu)
    vector<string> *headers = get_list_ex(ymake_headers, ymake_src_path);
    //dodanie œcie¿ki kompilatora do zmiennej œrodowiskowej PATH
    add_path(ymake_compiler_path);
    //utworzenie folderów
    if(!dir_exists("obj")) system2("mkdir obj");
    if(!dir_exists("prv")) system2("mkdir prv");
    if(!dir_exists("bin")) system2("mkdir bin");
    //znalezienie zmienionych plików h (dodatkowych plików projektu)
    bool rebuild = false, change = false;
    for(unsigned int i=0; i<headers->size(); i++){
        if(!file_exists(ymake_src_path + headers->at(i))){
            IO::error("brak pliku: "+ymake_src_path+headers->at(i));
            return false;
        }
        if(files_equal(ymake_src_path+headers->at(i),"prv\\"+headers->at(i)))
            continue;
        //przebudowa ca³ego projektu
        IO::echo("Zmodyfikowany plik: "+ymake_src_path+headers->at(i));
        rebuild = true;
        change = true;
        //skopiowanie nowej wersji z nadpisaniem
        if(!copy_files(ymake_src_path+headers->at(i),"prv\\"+headers->at(i))){
            IO::error("blad kopiowania do pliku prv\\"+headers->at(i));
            return false;
        }
    }
    delete headers;
    if(rebuild){
        IO::echo("Przebudowywanie...");
    }
    //znalezienie zmienionych plików cpp
    for(unsigned int i=0; i<srcs->size(); i++){
        if(!file_exists(ymake_src_path+srcs->at(i))){
            IO::error("brak pliku zrodlowego "+ymake_src_path+srcs->at(i));
            return false;
        }
        if(!rebuild){
            if(files_equal(ymake_src_path+srcs->at(i),"prv\\"+srcs->at(i)))
                continue;
            change = true;
        }
        //kompilacja zmienionych plików
        stringstream ss;
        ss<<ymake_compiler<<" -c -o \"obj\\"<<srcs->at(i)<<".o\" "<<ymake_src_path<<srcs->at(i);
        if(ymake_compiler_flags.length()>0) ss<<" "<<ymake_compiler_flags;
        IO::echo("Kompilacja "+srcs->at(i)+": "+ss.str());
        if(!system2(ss.str())){
            IO::error("blad kompilacji pliku: "+srcs->at(i));
            return false;
        }
        //skopiowanie nowej wersji z nadpisaniem
        if(!copy_files(ymake_src_path+srcs->at(i),"prv\\"+srcs->at(i))){
            IO::error("blad kopiowania do pliku prv\\"+srcs->at(i));
            return false;
        }
    }
    //Zasoby
    if(ymake_resource.length()>0){
        if(!file_exists(ymake_resource)){
            IO::error("brak pliku zasobow: "+ymake_resource);
            return false;
        }
        //sprawdzenie starej wersji
        if(!files_equal(ymake_resource,"prv\\"+ymake_resource) || !file_exists("obj\\"+ymake_resource+".o")){
            change = true;
            IO::echo("Dodawanie zasobow: "+ymake_resource);
            if(!system2("windres "+ymake_resource+" \"obj\\"+ymake_resource+".o\""))
                return false;
            if(!copy_files(ymake_resource,"prv\\"+ymake_resource)){
                IO::error("blad kopiowania do pliku prv\\"+ymake_resource);
                return false;
            }
        }
    }
    //inkrementacja wersji
    if(change || rebuild){
        if(ymake_version_file.length()>0){
            if(!version_inc(dir_format(ymake_version_file))){
                IO::error("blad inkrementacji wersji w pliku: "+ymake_version_file);
                return false;
            }
        }
        //konsolidacja ca³oœci aplikacji
        stringstream ss2;
        ss2<<ymake_compiler<<" -o \"bin\\"<<ymake_output<<"\"";
        for(unsigned int i=0; i<srcs->size(); i++){
            ss2<<" \"obj\\"<<srcs->at(i)<<".o\"";
        }
        if(ymake_resource.length()>0) ss2<<" \"obj\\"<<ymake_resource<<".o\"";
        if(ymake_libs.length()>0) ss2<<" "<<ymake_libs;
        if(ymake_linker_flags.length()>0) ss2<<" "<<ymake_linker_flags;
        IO::echo("Konsolidacja: "+ss2.str());
        if(!system2(ss2.str())){
            IO::error("blad konsolidacji aplikacji");
            return false;
        }
    }else{
        IO::echo("Brak zmian do wprowadzenia.");
    }
    delete srcs;
    return true;
}

bool ymake_generate_bat(string ymake_filename, string output_filename){
    ymake_filename = dir_format(ymake_filename);
    output_filename = dir_format(output_filename);
    stringstream output;
    vector<Variable*>* variables = get_variables(ymake_filename);
    if(variables==NULL){
        IO::error("brak poprawnego pliku \""+ymake_filename+"\"");
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
    //walidacja wczytanych danych, wartoœci domyœlne
    if(ymake_compiler.length()==0) ymake_compiler = "g++";
    if(ymake_output.length()==0) ymake_output = "main.exe";
    if(ymake_compiler_path.length()==0){
        IO::error("Brak sciezki do kompilatora (COMPILER_PATH)");
        return false;
    }
    if(ymake_src.length()==0){
        IO::error("Brak plikow zrodlowych (SRC)");
        return false;
    }
    //format ymake_src_path
    if(ymake_src_path.length()>0){
        if(ymake_src_path[ymake_src_path.length()-1]!='\\') ymake_src_path+='\\';
        if(ymake_src_path=="\\"||ymake_src_path==".\\") ymake_src_path = "";
    }
    //lista SRC
    vector<string> *srcs = get_list_ex(ymake_src, ymake_src_path);
    //dodanie œcie¿ki kompilatora do zmiennej œrodowiskowej PATH
    output<<"set PATH=%PATH%;"<<ymake_compiler_path<<endl;
    //utworzenie folderów
    output<<"mkdir obj"<<endl;
    output<<"mkdir bin"<<endl;
    //kompilacja plików cpp
    for(unsigned int i=0; i<srcs->size(); i++){
        //kompilacja plików cpp
        output<<ymake_compiler<<" -c -o \"obj\\"<<srcs->at(i)<<".o\" "<<ymake_src_path<<srcs->at(i);
        if(ymake_compiler_flags.length()>0) output<<" "<<ymake_compiler_flags;
        output<<endl;
    }
    //Zasoby
    if(ymake_resource.length()>0){
        output<<"windres "<<ymake_resource<<" \"obj\\"<<ymake_resource<<".o\""<<endl;
    }
    //konsolidacja ca³oœci aplikacji
    output<<ymake_compiler<<" -o \"bin\\"<<ymake_output<<"\"";
    for(unsigned int i=0; i<srcs->size(); i++){
        output<<" \"obj\\"<<srcs->at(i)<<".o\"";
    }
    delete srcs;
    if(ymake_resource.length()>0) output<<" \"obj\\"<<ymake_resource<<".o\"";
    if(ymake_libs.length()>0) output<<" "<<ymake_libs;
    if(ymake_linker_flags.length()>0) output<<" "<<ymake_linker_flags;
    output<<endl;
    //zapiasnie do pliku wyjœciowego
    ofstream plik;
    plik.open(output_filename.c_str());
    if(!plik.good()){
        plik.close();
        IO::error("Blad zapisywania do pliku: "+output_filename);
        return false;
    }
    plik<<output.str();
    plik.close();
    IO::echo("Wygenerowano plik: "+output_filename);
    return true;
}

bool ymake_generate_makefile(string ymake_filename, string output_filename){
    ymake_filename = dir_format(ymake_filename);
    output_filename = dir_format(output_filename);
    vector<Variable*>* variables = get_variables(ymake_filename);
    if(variables==NULL){
        IO::error("brak poprawnego pliku \""+ymake_filename+"\"");
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
    //walidacja wczytanych danych, wartoœci domyœlne
    if(ymake_compiler.length()==0) ymake_compiler = "g++";
    if(ymake_output.length()==0) ymake_output = "main.exe";
    if(ymake_compiler_path.length()==0){
        IO::error("Brak sciezki do kompilatora (COMPILER_PATH)");
        return false;
    }
    if(ymake_src.length()==0){
        IO::error("Brak plikow zrodlowych (SRC)");
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
        output<<" obj/"<<srcs->at(i)<<".o";
    }
    if(ymake_resource.length()>0){
        output<<" obj/"<<ymake_resource<<".o";
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
        output<<"obj/"<<srcs->at(i)<<".o: "<<ymake_src_path<<srcs->at(i)<<endl;
        output<<"\t$(CC) $(CFLAGS) "<<ymake_src_path<<srcs->at(i)<<" -o obj/"<<srcs->at(i)<<".o"<<endl<<endl;
    }
    delete srcs;
    if(ymake_resource.length()>0){
        output<<"obj/"<<ymake_resource<<".o: "<<ymake_resource<<endl;
        output<<"\t$(WINDRES) resource.rc \"obj/"<<ymake_resource<<".o\""<<endl<<endl;
    }
    //zapiasnie do pliku wyjœciowego
    ofstream plik;
    plik.open(output_filename.c_str());
    if(!plik.good()){
        plik.close();
        IO::error("Blad zapisywania do pliku: "+output_filename);
        return false;
    }
    plik<<output.str();
    plik.close();
    IO::echo("Wygenerowano plik: "+output_filename);
    return true;
}

bool run_ymake(string ymake_filename, int mode){
    ymake_filename = dir_format(ymake_filename);
    //odczytanie parametrów z pliku
    vector<Variable*>* variables = get_variables(ymake_filename);
    if(variables==NULL){
        IO::error("brak poprawnego pliku \""+ymake_filename+"\"");
        return false;
    }
    string ymake_output = get_var_string(variables, "OUTPUT");
    for(unsigned int i=0; i<variables->size(); i++){
        delete variables->at(i);
    }
    delete variables;
    //walidacja wczytanych danych, wartoœci domyœlne
    if(ymake_output.length()==0) ymake_output = "main.exe";
    //zmiana katalogu roboczego na ./bin
    if(!set_workdir(".\\bin")){
        IO::error("blad zmiany katalogu roboczego na bin\\");
        return false;
    }
    stringstream ss;
    if(mode==1){ // zwyk³y run - system
        ss<<"\""<<ymake_output<<"\"";
        IO::echo("Uruchamianie: "+ss.str());
        return system2(ss.str());
    }else if(mode==2){ //run start
        ss<<"start \"\" \""<<ymake_output<<"\"";
        IO::echo("Uruchamianie: "+ss.str());
        return system2(ss.str());
    }else if(mode==3){ //shelexecute
        if((int)ShellExecute(0, "open", ymake_output.c_str(), "", 0, SW_SHOW) > 32){
            IO::echo("Uruchomiono \""+ymake_output+"\"...");
            return true;
        }
        IO::error("Blad uruchamiania poleceniem ShellExecute: \""+ymake_output+"\"");
        return false;
    }
    return false;
}

bool clean_all(){
    if(!dir_exists("prv")){
        IO::info("brak folderu prv\\");
        return true;
    }
    IO::echo("Czyszczenie...");
    vector<string>* files = get_files_from_dir("prv");
    for(unsigned int i=0; i<files->size(); i++){
        IO::echo("Usuwanie: prv\\"+files->at(i));
        if(!delete_file("prv\\"+files->at(i))){
            IO::error("blad usuwania pliku prv\\"+files->at(i));
            return false;
        }
    }
    return true;
}
