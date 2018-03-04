#include "inheritancegraph.h"
#include <iostream>
#include <assert.h>
#include "stringtab.h"
#include <stack>
#include <algorithm>

void InheritanceGraph::add_edge(const Symbol v1, const Symbol v2)
{
    adj_li[v1].push_back(v2);
    transpose[v2].push_back(v1);
}


//TODO: make it iterative
bool InheritanceGraph::cycle_exists() const
{
    std::map < Symbol, char> color_map;
    for( map_sym_vec::const_iterator ad_it = adj_li.begin(); ad_it != adj_li.end(); ad_it++)
    {
        color_map[ad_it->first] = 'w';
    }
    
    for(auto ad_it = adj_li.begin(); ad_it != adj_li.end(); ad_it++)
    {
        if( color_map[ad_it->first] == 'w')
        {
            bool ret = dfs_visit(ad_it->first, color_map);
            if(ret)
            {
                return true;
            }
            //else continue
        }
    }
    
    return false;
}

bool InheritanceGraph::dfs_visit(const Symbol v, std::map <Symbol, char> &color_map) const
{
    color_map[v] = 'g';
    for(std::vector<Symbol>::const_iterator it = adj_li.at(v).begin(); it != adj_li.at(v).end(); it++)
    {
        if( color_map[*it] == 'g')
        {
            return true;
        }
        else if(color_map[*it] == 'w')
        {
            return dfs_visit(*it, color_map);
        }
    }
    
    color_map[v] = 'b';
    return false;
}

//following must be called only if graph is a tree
Symbol InheritanceGraph::join_of_types (const Symbol type1, const Symbol type2 ) const
{
    std::map < Symbol, char> color_map;
    Symbol ret = idtable.add_string("Object");
    //assert(transpose.count("Object") == 0);
    for(auto ad_it = transpose.begin(); ad_it != transpose.end(); ad_it++)
    {
        color_map[ad_it->first] = 'w';
       // assert(ad_it->second.size() == 1);
    }
    
    Symbol str = type1;
    color_map[str] = 'b';
    transpose.at(str);
    while(transpose.at(str).size() == 1)
    {
        
        str = *transpose.at(str).begin();
        color_map[str] = 'b';
    }
    assert(color_map[idtable.add_string("Object")] == 'b');
    
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
            std::cout << "Edge from " << ad_it->first->get_string() << " to " << (*li_it)->get_string() << "\n";
        }
    }
}

std::vector<Symbol> InheritanceGraph::pre_order() const
{
    
    std::vector <Symbol> ret;
    std::stack <Symbol> st;
    st.push(idtable.add_string("Object"));
    
    while(!st.empty())
    {
        Symbol top = st.top();
        ret.push_back(top);
        st.pop();
        try 
        {
            for(auto li_it = adj_li.at(top).rbegin(); li_it != adj_li.at(top).rend(); li_it++)
            {
                st.push(*li_it);
            }
        } catch(std::out_of_range e) {continue;}
    }
    
    
    return ret;
}

