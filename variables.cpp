#include "variables.h"
#include "files.h"
#include "io.h"

#include <cstdlib>

Variable::Variable(string name, string value){
    this->name = name;
    this->value = value;
}

vector<string>* get_variables_lines(string filename){
    vector<string>* lines = get_all_lines(filename);
    if(lines==NULL) return NULL;
    for(int i=0; i<(int)lines->size(); i++){
        if(lines->at(i).length()==0){ //usuni�cie pustych element�w
            lines->erase(lines->begin()+i);
            i--;
        }else if(lines->at(i).length()>=2){ //usuni�cie komentarzy
            if(lines->at(i)[0]=='/' && lines->at(i)[1]=='/'){
                lines->erase(lines->begin()+i);
                i--;
            }
        }
    }
    return lines;
}

vector<Variable*>* get_variables(string filename){
    vector<string>* lines = get_variables_lines(filename);
    if(lines==NULL) return NULL;
    vector<Variable*>* variables = new vector<Variable*>;
    //wy�uskanie nazwy i warto�ci
    for(unsigned int i=0; i<lines->size(); i++){
        //szukanie znaku =
        for(unsigned int j=1; j<lines->at(i).length(); j++){
            if(lines->at(i)[j] == '='){
                string name = lines->at(i).substr(0, j);
                string value = lines->at(i).substr(j+1);
                name = trim_spaces(name);
                value = trim_quotes(trim_spaces(value));
                variables->push_back(new Variable(name, value));
                break;
            }
        }
    }
    delete lines;
    return variables;
}

string get_var_string(vector<Variable*>* variables, string name){
    if(variables==NULL) return "";
    for(unsigned int i=0; i<variables->size(); i++){
        if(variables->at(i)->name == name){
            return variables->at(i)->value;
        }
    }
    IO::error("Nie znaleziono zmiennej: "+name);
    return "";
}

int get_var_int(vector<Variable*>* variables, string name){
    string s = get_var_string(variables, name);
    if(s.length()==0) return 0;
    return atoi(s.c_str());
}

bool get_var_bool(vector<Variable*>* variables, string name){
    string s = get_var_string(variables, name);
    if(s.length()==0) return false;
    if(s=="true") return true;
    if(s=="1") return true;
    return false;
}


string trim_spaces(string s){
    //obci�cie spacji na ko�cu
    while(s.length() > 0  && s[s.length()-1] == ' '){
        s = s.substr(0, s.length()-1);
    }
    //obci�cie spacji na pocz�tku
    while(s.length() > 0  && s[0] == ' '){
        s = s.substr(1);
    }
    return s;
}

string trim_quotes(string s){
    if(s.length()>=3){
        //je�li cudzys�owy s� na poczatku i na ko�cu
        if(s[0]=='\"' && s[s.length()-1]=='\"'){
            //je�li w ca�ym stringu znajduj� si� tylkko 2 cudzys�owy
            if(s.substr(1, s.length()-2).find('\"')==string::npos){
                s = s.substr(1, s.length()-2); //usu� je
            }
        }
    }
    return s;
}


vector<string>* get_list(string lista){
    vector<string>* kontener = new vector<string>;
    lista = trim_spaces(lista);
    if(lista.length()==0) return kontener;
    //podzia� przez spacje lub przecinki (nie przez cudzys�owy)
    bool cudzyslow = false;
    for(int i=0; i<(int)lista.length(); i++){
        if(lista[i]=='\"') cudzyslow = !cudzyslow;
        if(lista[i]==',' && !cudzyslow) lista[i] = ' ';
        if(lista[i]==' ' && !cudzyslow && i>0){
            kontener->push_back(lista.substr(0, i));
            lista = lista.substr(i+1);
            i = -1;
        }
    }
    if(lista.length()>0) kontener->push_back(lista);
    return kontener;
}

vector<string>* get_list_ex(string lista, string dir){
    vector<string>* kontener = get_list(lista);
    for(unsigned int i=0; i<kontener->size(); i++){
        //obetnij plusy z pocz�tku
        if(kontener->at(i)[0]=='+'){
            kontener->at(i).erase(0, 1);
        }
        //utnij cudzyslowy po obu stronach
        if(kontener->at(i).length()>=3 && kontener->at(i)[0]=='\"' && kontener->at(i)[kontener->at(i).length()-1]=='\"'){
            kontener->at(i) = kontener->at(i).substr(1, kontener->at(i).length()-2);
        }
    }
    //dodawanie element�w z gwiazdk�
    for(unsigned int i=0; i<kontener->size(); i++){
        if(kontener->at(i)[0]=='*'){ //dodaj wszystko (wszystko z podanym rozszerzeniem)
            string ext = kontener->at(i).substr(1, kontener->at(i).length()-1);
            //zast�p ten element pasuj�cymi plikami z folderu
            kontener->erase(kontener->begin() + i);
            vector<string>* pliki = get_files_from_dir(dir, ext);
            for(unsigned j=0; j<pliki->size(); j++){
                add_to_list(kontener, pliki->at(j));
            }
            delete pliki;
            i--;
        }
    }
    //odejmowanie element�w
    for(int i=0; i<(int)kontener->size(); i++){
        if(kontener->at(i)[0]=='-'){
            string elem_usun = kontener->at(i).substr(1, kontener->at(i).length()-1);
            //usu� ten element i wszystkie jego wyst�pienia
            kontener->erase(kontener->begin() + i);
            remove_from_list(kontener, elem_usun);
            //szukaj od nowa
            i = -1;
        }
    }
    return kontener;
}

void add_to_list(vector<string>* kontener, string elem){
    //je�li ju� istnieje
    for(unsigned int i=0; i<kontener->size(); i++){
        if(kontener->at(i)==elem)
            return;
    }
    kontener->push_back(elem);
}

void remove_from_list(vector<string>* kontener, string elem){
    //usu� wszystkie jego wyst�pienia
    for(int j=0; j<(int)kontener->size(); j++){
        if(kontener->at(j)==elem){
            kontener->erase(kontener->begin() + j);
            j--;
        }
    }
}
