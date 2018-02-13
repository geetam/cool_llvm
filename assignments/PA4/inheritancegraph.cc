#include "inheritancegraph.h"
#include <iostream>

void ingraph::add_edge(std::string v1, std::string v2)
{
    adj_li[v1].push_back(v2);
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


