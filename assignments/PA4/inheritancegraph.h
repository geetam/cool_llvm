#ifndef INHERITANCEGRAPH_H
#define INHERITANCEGRAPH_H

#include <map>
#include <vector>

typedef std::map < std::string, std::vector < std::string> > map_str_vec;
class InheritanceGraph {
private:
    map_str_vec adj_li;
    map_str_vec transpose;

    bool dfs_visit(const std::string &v) const;
public:
    void add_edge(const std::string &v1, const std::string &v2);
    bool cycle_exists() const;
    std::string join_of_types(const std::string &type1, const std::string &type2) const;
    void dump_edges() const;
};
#endif
