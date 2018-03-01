#include "inheritancegraph.h"
#include <iostream>
#include <assert.h>

void ingraph::add_edge(const std::string &v1, const std::string &v2)
{
    adj_li[v1].push_back(v2);
    transpose[v2].push_back(v1);
}


//TODO: make it iterative
bool ingraph::cycle_exists()
{
    for(auto ad_it = adj_li.begin(); ad_it != adj_li.end(); ad_it++)
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

bool ingraph::dfs_visit(std::string v)
{
    color_map[v] = 'g';
    for(auto it = adj_li[v].begin(); it != adj_li[v].end(); it++)
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
std::string ingraph::join_of_types(const std::string& type1, const std::string& type2)
{
    std::string ret = "Object";
    //assert(transpose.count("Object") == 0);
    for(auto ad_it = transpose.begin(); ad_it != transpose.end(); ad_it++)
    {
        color_map[ad_it->first] = 'w';
       // assert(ad_it->second.size() == 1);
    }
    
    std::string it = type1;
    color_map[it] = 'b';
    while(transpose[it].size() == 1)
    {
        
        it = *transpose[it].begin();
        color_map[it] = 'b';
    }
    assert(color_map["Object"] == 'b');
    
    if(color_map[type2] == 'b') {
        ret = type2;
    } else {
        it = type2;
        while(color_map[it] != 'b')
        {
            it = *transpose[it].begin();
        }
        ret = it;
    }
    
    return ret;
}


void ingraph::dump_edges()
{
    for(auto ad_it = adj_li.begin(); ad_it != adj_li.end(); ad_it++)
    {
        for(auto li_it = ad_it->second.begin(); li_it != ad_it->second.end(); li_it++)
        {
            std::cout << "Edge from " << ad_it->first << " to " << *li_it << "\n";
        }
    }
}
