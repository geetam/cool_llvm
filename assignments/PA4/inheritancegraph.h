#include <map>
#include <vector>


class ingraph {
private:
    std::map < std::string, std::vector < std::string> > adj_li;
    std::map < std::string, std::vector < std::string> > transpose;
    std::map < std::string, char> color_map;
    bool dfs_visit(std::string v);
public:
    void add_edge(std::string v1, std::string v2);
    bool cycle_exists();
    std::string join_of_types(const std::string &type1, const std::string &type2);
    void dump_edges();
};