#include "environment.h"
#include <list>

Environment::Environment() : built_method_env(false)
{
}

std::map<std::pair<Symbol, Symbol>, type_vec> Environment::get_method_env()
{
    if(built_method_env)
    {
        return method_env;
    }
    else
    {
        for(auto map_it = classname_methods_map.begin(); map_it != classname_methods_map.end(); map_it++)
        {
            Symbol classname = map_it->first;
            class_methods meths = map_it->second;
            auto methods = meths.get_entries();
            for(auto method : methods)
            {
                method_env[std::make_pair(classname, method.first)] = method.second;
            }
        }
        built_method_env = true;
        return method_env;
    }
}



