#ifndef ENVIRONEMENT_H
#define ENVIRONEMENT_H

#include "symtab.h"
#include "inheritancegraph.h"
#include "stringtab.h"

class Environment {
public:
    InheritanceGraph igraph;
    SymbolTable <Symbol, Symbol> symbol_table;
    Symbol current_class;
};

#endif
