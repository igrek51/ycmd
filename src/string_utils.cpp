#include "string_utils.h"

string trim_spaces(string s){
    //obcięcie spacji na końcu
    while(s.length() > 0  && s[s.length()-1] == ' '){
        s = s.substr(0, s.length()-1);
    }
    //obcięcie spacji na początku
    while(s.length() > 0  && s[0] == ' '){
        s = s.substr(1);
    }
    return s;
}

string trim_quotes(string s){
    if(s.length()>=3){
        //jeśli cudzysłowy są na poczatku i na końcu
        if(s[0]=='\"' && s[s.length()-1]=='\"'){
            //jeśli w całym stringu znajdują się tylko 2 cudzysłowy
            if(s.substr(1, s.length()-2).find('\"')==string::npos){
                s = s.substr(1, s.length()-2); //usuń je
            }
        }
    }
    return s;
}


vector<string>* get_param_list(string lista){
    vector<string>* kontener = new vector<string>;
    lista = trim_spaces(lista);
    if(lista.length()==0) return kontener;
    //podział przez spacje lub przecinki (nie przez cudzysłowy)
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
