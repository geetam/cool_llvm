#ifndef ENVIRONEMENT_H
#define ENVIRONEMENT_H

#include "symtab.h"
#include "inheritancegraph.h"
#include "stringtab.h"

class Environment {
private:
    std::map < std::pair <Symbol, Symbol>, type_vec> method_env;
    bool built_method_env;
public:
    Environment();
    std::map < std::pair <Symbol, Symbol>, type_vec> get_method_env();
    
    InheritanceGraph igraph;
    symtab_sym_dat symbol_table;
    std::map <Symbol, class_atts> classname_atts_map;
    std::map <Symbol, class_methods> classname_methods_map;
    Symbol current_class;
    
};

#endif
