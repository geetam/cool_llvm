#ifndef INHERITANCEGRAPH_H
#define INHERITANCEGRAPH_H

#include <map>
#include <vector>
#include "stringtab.h"

typedef std::map < Symbol, std::vector < Symbol> > map_sym_vec;
class InheritanceGraph {
private:
    map_sym_vec adj_li;
    map_sym_vec transpose;

    bool dfs_visit(const Symbol v, std::map <Symbol, char> &color_map) const;
public:
    void add_edge(const Symbol v1, const Symbol v2);
    bool cycle_exists() const;
    Symbol join_of_types(const Symbol type1, const Symbol type2) const;
    void dump_edges() const;
};
#endif
