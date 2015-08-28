#include "variable.h"
#include "files.h"
#include <cstdlib>

Variable::Variable(string name, string value){
    this->name = name;
    this->value = value;
}

vector<string>* get_variables_lines(string filename){
    vector<string>* lines = get_all_lines(filename);
    if(lines==NULL) return NULL;
    for(int i=0; i<(int)lines->size(); i++){
        if(lines->at(i).length()==0){ //usuniêcie pustych elementów
            lines->erase(lines->begin()+i);
            i--;
        }else if(lines->at(i).length()>=2){ //usuniêcie komentarzy
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
    //wy³uskanie nazwy i wartoœci
    for(unsigned int i=0; i<lines->size(); i++){
        //szukanie znaku =
        for(unsigned int j=1; j<lines->at(i).length(); j++){
            if(lines->at(i)[j] == '='){
                string name = lines->at(i).substr(0, j);
                string value = lines->at(i).substr(j+1);
                name = trim_spaces(name);
                value = trim_spaces(value);
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
    cout<<"[BLAD!] Nie znaleziono zmiennej: "<<name;
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
    //obciêcie spacji na koñcu
    while(s.length() > 0  && s[s.length()-1] == ' '){
        s = s.substr(0, s.length()-1);
    }
    //obciêcie spacji na pocz¹tku
    while(s.length() > 0  && s[0] == ' '){
        s = s.substr(1);
    }
    return s;
}
