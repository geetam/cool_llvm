#ifndef ENVIRONEMENT_H
#define ENVIRONEMENT_H

#include "symtab.h"
#include "inheritancegraph.h"
#include "stringtab.h"

class Environment {
public:
    InheritanceGraph igraph;
    symtab_sym_dat symbol_table;
    std::map <Symbol, class_atts> classname_atts_map;
    std::map <Symbol, class_methods> classname_methods_map;
    Symbol current_class;
};

#endif
