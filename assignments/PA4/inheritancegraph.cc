#include "inheritancegraph.h"
#include <iostream>
#include <assert.h>

void InheritanceGraph::add_edge(const std::string &v1, const std::string &v2)
{
    adj_li[v1].push_back(v2);
    transpose[v2].push_back(v1);
}


//TODO: make it iterative
bool InheritanceGraph::cycle_exists() const
{
    std::map < std::string, char> color_map;
    for(map_str_vec::const_iterator ad_it = adj_li.begin(); ad_it != adj_li.end(); ad_it++)
    {
        color_map[ad_it->first] = 'w';
    }
    
    for(auto ad_it = adj_li.begin(); ad_it != adj_li.end(); ad_it++)
    {
        if( color_map[ad_it->first] == 'w')
        {
            bool ret = dfs_visit(ad_it->first);
            if(ret)
            {
                return true;
            }
            //else continue
        }
    }
    
    return false;
}

bool InheritanceGraph::dfs_visit(const std::string &v) const
{
    std::map < std::string, char> color_map;
    color_map[v] = 'g';
    for(std::vector<std::string>::const_iterator it = adj_li.at(v).begin(); it != adj_li.at(v).end(); it++)
    {
        if( color_map[*it] == 'g')
        {
            return true;
        }
        else if(color_map[*it] == 'w')
        {
            return dfs_visit(*it);
        }
    }
    
    color_map[v] = 'b';
    return false;
}

//following must be called only if graph is a tree
std::string InheritanceGraph::join_of_types (const std::string& type1, const std::string& type2 ) const
{
    std::map < std::string, char> color_map;
    std::string ret = "Object";
    //assert(transpose.count("Object") == 0);
    for(auto ad_it = transpose.begin(); ad_it != transpose.end(); ad_it++)
    {
        color_map[ad_it->first] = 'w';
       // assert(ad_it->second.size() == 1);
    }
    
    std::string str = type1;
    color_map[str] = 'b';
    transpose.at(str);
    while(transpose.at(str).size() == 1)
    {
        
        str = *transpose.at(str).begin();
        color_map[str] = 'b';
    }
    assert(color_map["Object"] == 'b');
    
    if(color_map[type2] == 'b') {
        ret = type2;
    } else {
        str = type2;
        while(color_map[str] != 'b')
        {
            str = *transpose.at(str).begin();
        }
        ret = str;
    }
    
    return ret;
}


void InheritanceGraph::dump_edges() const
{
    for(auto ad_it = adj_li.begin(); ad_it != adj_li.end(); ad_it++)
    {
        for(auto li_it = ad_it->second.begin(); li_it != ad_it->second.end(); li_it++)
        {
            std::cout << "Edge from " << ad_it->first << " to " << *li_it << "\n";
        }
    }
}
