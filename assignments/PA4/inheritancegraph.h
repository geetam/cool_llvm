#ifndef INHERITANCEGRAPH_H
#define INHERITANCEGRAPH_H

#include <map>
#include <vector>


class InheritanceGraph {
private:
    std::map < std::string, std::vector < std::string> > adj_li;
    std::map < std::string, std::vector < std::string> > transpose;
    std::map < std::string, char> color_map;
    bool dfs_visit(std::string v);
public:
    void add_edge(const std::string &v1, const std::string &v2);
    bool cycle_exists();
    std::string join_of_types(const std::string &type1, const std::string &type2);
    void dump_edges();
};
#endif
