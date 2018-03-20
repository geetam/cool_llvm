//////////////////////////////////////////////////////////
//
// file: cool-tree.cc
//
// This file defines the functions of each class
//
//////////////////////////////////////////////////////////

#include <set>
#include "tree.h"
#include "cool-tree.handcode.h"
#include "cool-tree.h"
#include "semanterror.h"
#include <algorithm>

extern SemantError serror;
// constructors' functions
Program program_class::copy_Program()
{
   return new program_class(classes->copy_list());
}


void program_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "program\n";
   classes->dump(stream, n+2);
}


Class_ class__class::copy_Class_()
{
   return new class__class(copy_Symbol(name), copy_Symbol(parent), features->copy_list(), copy_Symbol(filename));
}


void class__class::dump(ostream& stream, int n)
{
   stream << pad(n) << "class_\n";
   dump_Symbol(stream, n+2, name);
   dump_Symbol(stream, n+2, parent);
   features->dump(stream, n+2);
   dump_Symbol(stream, n+2, filename);
}

Symbol class__class::check_type(const Environment &env)
{
    class_atts clssym = env.classname_atts_map.find(name)->second;
    std::list < pair_att_entry> entries = clssym.get_entries();
    Environment env_mod = env;
    env_mod.symbol_table.enterscope();
    for(auto it = entries.begin(); it != entries.end(); it++)
    {
        Symbol name_att = it->first;
        Symbol type_att = it->second.get_typedec();
        Symbol actual_type_att = type_att == idtable.add_string("SELF_TYPE") ? name : type_att;
        env_mod.symbol_table.addid(name_att, new SymEntryData(actual_type_att));
    }
    
    
    for(int i = features->first(); features->more(i); i++)
    {
        features->nth(i)->check_type(env_mod);
    }
    env_mod.symbol_table.exitscope();
}

std::pair <class_atts, class_methods> class__class::gen_class_symtab()
{
    class_atts ret_att;
    class_methods ret_meth;
    for(int i = features->first(); features->more(i); i++)
    {
        attr_class *att = dynamic_cast <attr_class*>(features->nth(i));
        method_class *meth = dynamic_cast <method_class*>(features->nth(i));
        
        if(att)
        {
            SymEntryData dat(att->getTypeDec());
            ret_att.add_entry(att->getName(), dat);
        }
        else if(meth)
        {
            type_vec typs;
            Formals formals = meth->getFormals();
            for(int i = formals->first(); formals->more(i); i = formals->next(i))
            {
                formal_class *fp = static_cast <formal_class*> (formals->nth(i));
                typs.push_back(fp->getTypeDec());
            }
            
            typs.push_back(meth->getRetTypeDec());
             
            ret_meth.add_entry(meth->getName(), typs);
            
        }
        
    }
    
    return std::make_pair(ret_att, ret_meth);
}

Feature method_class::copy_Feature()
{
   return new method_class(copy_Symbol(name), formals->copy_list(), copy_Symbol(return_type), expr->copy_Expression());
}


void method_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "method\n";
   dump_Symbol(stream, n+2, name);
   formals->dump(stream, n+2);
   dump_Symbol(stream, n+2, return_type);
   expr->dump(stream, n+2);
}

Symbol method_class::check_type(const Environment& env)
{
    Environment env_mod = env;
    
    env_mod.symbol_table.addid(idtable.add_string("self"), new SymEntryData(env_mod.current_class));
    for(int i = formals->first(); formals->more(i); formals->next(i))
    {
        formal_class *formal = static_cast <formal_class*>(formals->nth(i));
        env_mod.symbol_table.addid(formal->getName(), new SymEntryData(formal->getTypeDec()));
    }
    
    Symbol type_expr = expr->check_type(env_mod);
    Symbol actual_return_type = return_type == idtable.add_string("SELF_TYPE") ? env_mod.current_class : return_type;
    
    if(env_mod.igraph.join_of_types(type_expr, actual_return_type) != actual_return_type)
    {
        std::string errmsg = "in method: \"" + std::string(name->get_string())
                           + "\" expression's type \"" + std::string(type_expr->get_string()) 
                           + "\" does not conform to the return type \""
                           + std::string(actual_return_type->get_string()) + "\"";
        serror.print_error(get_line_number(), errmsg);
    }
    return actual_return_type;
}


Feature attr_class::copy_Feature()
{
   return new attr_class(copy_Symbol(name), copy_Symbol(type_decl), init->copy_Expression());
}


void attr_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "attr\n";
   dump_Symbol(stream, n+2, name);
   dump_Symbol(stream, n+2, type_decl);
   init->dump(stream, n+2);
}

Symbol attr_class::check_type(const Environment &env)
{
    no_expr_class *no_expr_ptr = dynamic_cast<no_expr_class*>(init);
    if(no_expr_ptr == nullptr) //init is NOT no_expr
    {
        Environment env_mod = env;
        Symbol actual_type = type_decl == idtable.add_string("SELF_TYPE") ? env.current_class : type_decl;
        env_mod.symbol_table.addid(idtable.add_string("self"), new SymEntryData(env.current_class));
        env_mod.symbol_table.addid(name, new SymEntryData(actual_type));
        
        Symbol type_init = init->check_type(env_mod);
        if(env.igraph.join_of_types(type_init, actual_type) != actual_type) //type_init does not conform to actual_type
        {
            std::string errmsg = "attribute of type \"" + std::string(actual_type->get_string()) + "\" " + 
                                 "cannot be assigned an expression of type \"" + std::string(type_init->get_string()) + "\" ";
            serror.print_error(get_line_number(), errmsg);
            type = idtable.add_string("Object");
        }
        else
        {
            type = actual_type;
        }
    }
    else //init is no_expr
    { 
        type = type_decl;
    }
    
    return type;
}



Formal formal_class::copy_Formal()
{
   return new formal_class(copy_Symbol(name), copy_Symbol(type_decl));
}


void formal_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "formal\n";
   dump_Symbol(stream, n+2, name);
   dump_Symbol(stream, n+2, type_decl);
}


Case branch_class::copy_Case()
{
   return new branch_class(copy_Symbol(name), copy_Symbol(type_decl), expr->copy_Expression());
}


void branch_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "branch\n";
   dump_Symbol(stream, n+2, name);
   dump_Symbol(stream, n+2, type_decl);
   expr->dump(stream, n+2);
}

Symbol branch_class::check_type(const Environment& env)
{
    type = expr->check_type(env);
    return type;
}


Expression assign_class::copy_Expression()
{
   return new assign_class(copy_Symbol(name), expr->copy_Expression());
}


void assign_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "assign\n";
   dump_Symbol(stream, n+2, name);
   expr->dump(stream, n+2);
}


Expression static_dispatch_class::copy_Expression()
{
   return new static_dispatch_class(expr->copy_Expression(), copy_Symbol(type_name), copy_Symbol(name), actual->copy_list());
}


void static_dispatch_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "static_dispatch\n";
   expr->dump(stream, n+2);
   dump_Symbol(stream, n+2, type_name);
   dump_Symbol(stream, n+2, name);
   actual->dump(stream, n+2);
}


Symbol static_dispatch_class::check_type(const Environment& env)
{
    Environment env_copy = env;
    Symbol expr_type = expr->check_type(env);
    Symbol class_of_meth = type_name;
    if(env.igraph.join_of_types(expr_type, class_of_meth) != class_of_meth)
    {
        std::string errmsg = "expr type \"" + std::string(expr_type->get_string())
                           + "\" does not conform to static type \"" + std::string(class_of_meth->get_string())
                           + "\"";
        serror.print_error(get_line_number(), errmsg);
    }
    
    auto key = std::make_pair(class_of_meth, name);
    
    if(env_copy.get_method_env().count(key) == 0)
    {
        std::string errmsg = "No function with name \"" + std::string(name->get_string())
                            + "\" declared in class \"" + std::string(class_of_meth->get_string()) + "\"";
        serror.print_error(get_line_number(), errmsg);
        type = idtable.add_string("Object");
    }
    else
    {
        type_vec actual_typs;
        actual_typs.push_back(class_of_meth);
        for(int i = actual->first(); actual->more(i); i = actual->next(i))
        {
            actual_typs.push_back(actual->nth(i)->check_type(env));
        }
        
        type_vec method_signature = env_copy.get_method_env().find(key)->second;
        if(method_signature.size() != actual_typs.size())
        {
            std::string errmsg =  "method \"" + std::string(name->get_string())+ "\" expected " + std::to_string(method_signature.size() -1)
                                +  " arguments but " + std::to_string(actual_typs.size() - 1) + " were supplied";
            serror.print_error(get_line_number(), errmsg);
            type = idtable.add_string("Object");
        }
        else
        {
            int n = actual_typs.size();
            bool ok = true;
            for(int i = 1; i < n; i++)
            {
                Symbol method_signature_curr = method_signature[i - 1] == idtable.add_string("SELF_TYPE") ?
                                               class_of_meth : method_signature[i - 1];
                if(env.igraph.join_of_types(actual_typs[i], method_signature_curr) == method_signature_curr)
                {
                    continue;
                }
                else
                {
                    std::string errmsg = "Supplied argument at position " + std::to_string(i+1)+"\""
                                         + std::string(actual_typs[i]->get_string()) + "\" does not conform to"
                                         + "required type \"" + std::string(method_signature_curr->get_string())
                                         + "\"";
                    serror.print_error(get_line_number(), errmsg);
                    type = idtable.add_string("Object");
                    ok = false;
                }
            }
            
            if(ok)
            {
                Symbol rettype = *(method_signature.end() - 1);
                type = rettype == idtable.add_string("SELF_TYPE") ? *(actual_typs.begin()) : rettype;
            }
        }
        
        
    }
    
    return type;
}


Expression dispatch_class::copy_Expression()
{
   return new dispatch_class(expr->copy_Expression(), copy_Symbol(name), actual->copy_list());
}


void dispatch_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "dispatch\n";
   expr->dump(stream, n+2);
   dump_Symbol(stream, n+2, name);
   actual->dump(stream, n+2);
}

Symbol dispatch_class::check_type(const Environment& env)
{
    Environment env_copy = env;
    Symbol class_of_meth = expr->check_type(env);
    class_of_meth = class_of_meth == idtable.add_string("SELF_TYPE") ? env.current_class : class_of_meth;
    auto key = std::make_pair(class_of_meth, name);
    
    if(env_copy.get_method_env().count(key) == 0)
    {
        std::string errmsg = "No function with name \"" + std::string(name->get_string())
                            + "\" declared in class \"" + std::string(class_of_meth->get_string()) + "\"";
        serror.print_error(get_line_number(), errmsg);
        type = idtable.add_string("Object");
    }
    else
    {
        type_vec actual_typs;
        actual_typs.push_back(class_of_meth);
        for(int i = actual->first(); actual->more(i); i = actual->next(i))
        {
            actual_typs.push_back(actual->nth(i)->check_type(env));
        }
        
        type_vec method_signature = env_copy.get_method_env().find(key)->second;
        if(method_signature.size() != actual_typs.size())
        {
            std::string errmsg =  "method \"" + std::string(name->get_string())+ "\" expected " + std::to_string(method_signature.size() -1)
                                +  " arguments but " + std::to_string(actual_typs.size() - 1) + " were supplied";
            serror.print_error(get_line_number(), errmsg);
            type = idtable.add_string("Object");
        }
        else
        {
            int n = actual_typs.size();
            bool ok = true;
            for(int i = 1; i < n; i++)
            {
                Symbol method_signature_curr = method_signature[i - 1] == idtable.add_string("SELF_TYPE") ?
                                               class_of_meth : method_signature[i - 1];
                if(env.igraph.join_of_types(actual_typs[i], method_signature_curr) == method_signature_curr)
                {
                    continue;
                }
                else 
                {
                    std::string errmsg = "Supplied argument at position " + std::to_string(i+1)+"\""
                                        + std::string(actual_typs[i]->get_string()) + "\" does not conform to"
                                        + "required type \"" + std::string(method_signature_curr->get_string())
                                        + "\"";
                    serror.print_error(get_line_number(), errmsg);
                    type = idtable.add_string("Object");
                    ok = false;
                }
            }
            
            if(ok)
            {
                Symbol rettype = *(method_signature.end() - 1);
                type = rettype == idtable.add_string("SELF_TYPE") ? *(actual_typs.begin()) : rettype;
            }
        }
        
        
    }
    
    return type;
}


Expression cond_class::copy_Expression()
{
   return new cond_class(pred->copy_Expression(), then_exp->copy_Expression(), else_exp->copy_Expression());
}


void cond_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "cond\n";
   pred->dump(stream, n+2);
   then_exp->dump(stream, n+2);
   else_exp->dump(stream, n+2);
}

Symbol cond_class::check_type(const Environment& env)
{
    Symbol pred_type = pred->check_type(env);
    if(pred_type != idtable.add_string("Bool"))
    {
        serror.print_error(get_line_number(), "Predicate expression must be of type Bool");
    }
    Symbol then_type = then_exp->check_type(env);
    Symbol else_type = else_exp->check_type(env);
    
    type = env.igraph.join_of_types(then_type, else_type);
    return type;
    
}


Expression loop_class::copy_Expression()
{
   return new loop_class(pred->copy_Expression(), body->copy_Expression());
}


void loop_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "loop\n";
   pred->dump(stream, n+2);
   body->dump(stream, n+2);
}

Symbol loop_class::check_type(const Environment& env)
{
    Symbol pred_type = pred->check_type(env);
    if(pred_type != idtable.add_string("Bool"))
    {
        serror.print_error(get_line_number(), "Predicate of a loop must be of type Bool");
    }
    body->check_type(env);
    type = idtable.add_string("Object");
    return type;
}


Expression typcase_class::copy_Expression()
{
   return new typcase_class(expr->copy_Expression(), cases->copy_list());
}


void typcase_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "typcase\n";
   expr->dump(stream, n+2);
   cases->dump(stream, n+2);
}

Symbol typcase_class::check_type(const Environment& env)
{
    expr->check_type(env);
    type_vec expr_types;
    type_vec decl_types;
    for(int i = cases->first(); cases->more(i); i = cases->next(i))
    {
        Environment env_mod = env;
        env_mod.symbol_table.enterscope();
        branch_class *branch_p = static_cast <branch_class*>(cases->nth(i));
        env_mod.symbol_table.addid(branch_p->getName(), new SymEntryData(branch_p->getTypeDec()));
        expr_types.push_back(branch_p->check_type(env_mod));
        decl_types.push_back(branch_p->getTypeDec());
        env_mod.symbol_table.exitscope();
    }
    auto it = std::unique(decl_types.begin(), decl_types.end());
    if(it != decl_types.end())
    {
        serror.print_error(get_line_number(), "No two branches may have same declared type");
    }

    Symbol ret = expr_types[0];
    for(Symbol etyp : expr_types)
    {
        ret = env.igraph.join_of_types(ret, etyp);
    }
    type = ret;
    return type;
    
}


Expression block_class::copy_Expression()
{
   return new block_class(body->copy_list());
}


void block_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "block\n";
   body->dump(stream, n+2);
}

Symbol block_class::check_type(const Environment& env)
{
    for(int i = body->first(); body->more(i); i = body->next(i))
    {
        type = body->nth(i)->check_type(env);
    }
    return type;
}

Expression let_class::copy_Expression()
{
   return new let_class(copy_Symbol(identifier), copy_Symbol(type_decl), init->copy_Expression(), body->copy_Expression());
}


void let_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "let\n";
   dump_Symbol(stream, n+2, identifier);
   dump_Symbol(stream, n+2, type_decl);
   init->dump(stream, n+2);
   body->dump(stream, n+2);
}

Symbol let_class::check_type(const Environment& env)
{
    Symbol init_type = init->check_type(env);
    Symbol actual_type_decl = type_decl == idtable.add_string("SELF_TYPE") ? env.current_class : type_decl;
    if(init_type != idtable.add_string("No_type"))
    {
        if(env.igraph.join_of_types(init_type, actual_type_decl) != actual_type_decl)
        {
            serror.print_error(get_line_number(), "let: type of init expr does not conform to the declared type");
        }
    }
    Environment env_mod = env;
    env_mod.symbol_table.enterscope();
    env_mod.symbol_table.addid(identifier, new SymEntryData(actual_type_decl));
    
    type = body->check_type(env_mod);
    env_mod.symbol_table.exitscope();
    return type;
}

Expression plus_class::copy_Expression()
{
   return new plus_class(e1->copy_Expression(), e2->copy_Expression());
}


void plus_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "plus\n";
   e1->dump(stream, n+2);
   e2->dump(stream, n+2);
}

Symbol plus_class::check_type(const Environment &env)
{
    std::string type1 = e1->check_type(env)->get_string();
    std::string type2 = e2->check_type(env)->get_string();
    
    bool ok = true;
    if(type1 != "Int")
    {
        serror.print_error(get_line_number(),"type of left expression is NOT Int");
        ok = false;
    }
    
    if(type2 != "Int")
    {
        serror.print_error(get_line_number(), "type of right expression is NOT Int");
        ok = false;
    }
    
    if(ok)
    {
        type = idtable.add_string("Int");
    } else {
        type = idtable.add_string("Object");
    }
    
    return type;
        
}


Expression sub_class::copy_Expression()
{
   return new sub_class(e1->copy_Expression(), e2->copy_Expression());
}


void sub_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "sub\n";
   e1->dump(stream, n+2);
   e2->dump(stream, n+2);
}

Symbol sub_class::check_type(const Environment &env)
{
    std::string type1 = e1->check_type(env)->get_string();
    std::string type2 = e2->check_type(env)->get_string();
    
    bool ok = true;
    if(type1 != "Int")
    {
        serror.print_error(get_line_number(),"type of left expression is NOT Int");
        ok = false;
    }
    
    if(type2 != "Int")
    {
        serror.print_error(get_line_number(), "type of right expression is NOT Int");
        ok = false;
    }
    
    if(ok)
    {
        type = idtable.add_string("Int");
    } else {
        type = idtable.add_string("Object");
    }
    
    return type;
        
}



Expression mul_class::copy_Expression()
{
   return new mul_class(e1->copy_Expression(), e2->copy_Expression());
}


void mul_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "mul\n";
   e1->dump(stream, n+2);
   e2->dump(stream, n+2);
}

Symbol mul_class::check_type(const Environment &env)
{
    std::string type1 = e1->check_type(env)->get_string();
    std::string type2 = e2->check_type(env)->get_string();
    
    bool ok = true;
    if(type1 != "Int")
    {
        serror.print_error(get_line_number(),"type of left expression is NOT Int");
        ok = false;
    }
    
    if(type2 != "Int")
    {
        serror.print_error(get_line_number(), "type of right expression is NOT Int");
        ok = false;
    }
    
    if(ok)
    {
        type = idtable.add_string("Int");
    } else {
        type = idtable.add_string("Object");
    }
    
    return type;
        
}


Expression divide_class::copy_Expression()
{
   return new divide_class(e1->copy_Expression(), e2->copy_Expression());
}


void divide_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "divide\n";
   e1->dump(stream, n+2);
   e2->dump(stream, n+2);
}

Symbol divide_class::check_type(const Environment &env)
{
    std::string type1 = e1->check_type(env)->get_string();
    std::string type2 = e2->check_type(env)->get_string();
    
    bool ok = true;
    if(type1 != "Int")
    {
        serror.print_error(get_line_number(),"type of left expression is NOT Int");
        ok = false;
    }
    
    if(type2 != "Int")
    {
        serror.print_error(get_line_number(), "type of right expression is NOT Int");
        ok = false;
    }
    
    if(ok)
    {
        type = idtable.add_string("Int");
    } else {
        type = idtable.add_string("Object");
    }
    
    return type;
        
}

Expression neg_class::copy_Expression()
{
   return new neg_class(e1->copy_Expression());
}


void neg_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "neg\n";
   e1->dump(stream, n+2);
}

Symbol neg_class::check_type(const Environment &env)
{
    if(strcmp(e1->check_type(env)->get_string(), "Bool") != 0)
    {
        serror.print_error(get_line_number(), "negation used with a non bool");
        type = idtable.add_string("Object");
    } else {
        type = idtable.add_string("Bool");
    }
    
    return type;
}

Expression lt_class::copy_Expression()
{
   return new lt_class(e1->copy_Expression(), e2->copy_Expression());
}


void lt_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "lt\n";
   e1->dump(stream, n+2);
   e2->dump(stream, n+2);
}

Symbol lt_class::check_type(const Environment &env)
{
    std::string type1 = e1->check_type(env)->get_string();
    std::string type2 = e2->check_type(env)->get_string();
    
    bool ok = true;
    if(type1 != "Int")
    {
        serror.print_error(get_line_number(), "type of left expression of '<' operator isn't Int");
        ok = false;
    }
    if(type2 != "Int")
    {
        serror.print_error(get_line_number(), "type of right expression of '<' operator isn't Int");
        ok = false;
    }
    
    if(ok)
    {
        type = idtable.add_string("Bool");
    } else {
        type = idtable.add_string("Object");
    }
    
    return type;
}


Expression eq_class::copy_Expression()
{
   return new eq_class(e1->copy_Expression(), e2->copy_Expression());
}


void eq_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "eq\n";
   e1->dump(stream, n+2);
   e2->dump(stream, n+2);
}

Symbol eq_class::check_type(const Environment &env)
{
    std::string type1 = e1->check_type(env)->get_string();
    std::string type2 = e2->check_type(env)->get_string();
    //being optimistic here
    type = idtable.add_string("Bool");

    //in case t1 or t2 is one of Int, String or bool and t1 != t2, report error.
    std::set <std::string> basic_types = {"Int", "String", "Bool"};
    
    if(basic_types.count(type1) || basic_types.count(type2) )
    {
        if(type1 != type2)
        {
            serror.print_error(get_line_number(), "The expressions being compared are not of same type,\n"\
                                                  "and one of the types is Int, String or Bool" );
            type = idtable.add_string("Object");
        }
    }
    
    return type;
    
}

Expression leq_class::copy_Expression()
{
   return new leq_class(e1->copy_Expression(), e2->copy_Expression());
}


void leq_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "leq\n";
   e1->dump(stream, n+2);
   e2->dump(stream, n+2);
}

Symbol leq_class::check_type(const Environment &env)
{
    std::string type1 = e1->check_type(env)->get_string();
    std::string type2 = e2->check_type(env)->get_string();
    
    bool ok = true;
    if(type1 != "Int")
    {
        serror.print_error(get_line_number(), "type of left expression of '<=' operator isn't Int");
        ok = false;
    }
    if(type2 != "Int")
    {
        serror.print_error(get_line_number(), "type of right expression of '<=' operator isn't Int");
        ok = false;
    }
    
    if(ok)
    {
        type = idtable.add_string("Bool");
    } else {
        type = idtable.add_string("Object");
    }
    
    return type;
}

Expression comp_class::copy_Expression()
{
   return new comp_class(e1->copy_Expression());
}

Symbol comp_class::check_type(const Environment &env)
{
    if(strcmp(e1->check_type(env)->get_string(), "Bool") != 0)
    {
        serror.print_error(get_line_number(), " \"Not\" used with a non bool");
        type = idtable.add_string("Object");
    } else {
        type = idtable.add_string("Bool");
    }
    
    return type;
}


void comp_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "comp\n";
   e1->dump(stream, n+2);
}


Expression int_const_class::copy_Expression()
{
   return new int_const_class(copy_Symbol(token));
}


void int_const_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "int_const\n";
   dump_Symbol(stream, n+2, token);
}

Symbol int_const_class::check_type(const Environment &env)
{
    type = idtable.add_string("Int");
    return type;
}

Expression bool_const_class::copy_Expression()
{
   return new bool_const_class(copy_Boolean(val));
}


void bool_const_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "bool_const\n";
   dump_Boolean(stream, n+2, val);
}

Symbol bool_const_class::check_type(const Environment &env)
{
    type = idtable.add_string("Bool");
    return type;
}


Expression string_const_class::copy_Expression()
{
   return new string_const_class(copy_Symbol(token));
}

Symbol string_const_class::check_type(const Environment &env)
{
    type = idtable.add_string("String");
    return type;
}


void string_const_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "string_const\n";
   dump_Symbol(stream, n+2, token);
}


Expression new__class::copy_Expression()
{
   return new new__class(copy_Symbol(type_name));
}


void new__class::dump(ostream& stream, int n)
{
   stream << pad(n) << "new_\n";
   dump_Symbol(stream, n+2, type_name);
}

Symbol new__class::check_type(const Environment& env)
{
    type = type_name == idtable.add_string("SELF_TYPE") ? env.current_class : type_name;
    return type;
}


Expression isvoid_class::copy_Expression()
{
   return new isvoid_class(e1->copy_Expression());
}


void isvoid_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "isvoid\n";
   e1->dump(stream, n+2);
}

Symbol isvoid_class::check_type(const Environment &env)
{
    e1->check_type(env);
    
    type = idtable.add_string("Bool");
    return type;
}

Expression no_expr_class::copy_Expression()
{
   return new no_expr_class();
}

Symbol no_expr_class::check_type(const Environment &env)
{
    type = idtable.add_string("No_type");
    return type;
}


void no_expr_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "no_expr\n";
}


Expression object_class::copy_Expression()
{
   return new object_class(copy_Symbol(name));
}


void object_class::dump(ostream& stream, int n)
{
   stream << pad(n) << "object\n";
   dump_Symbol(stream, n+2, name);
}

Symbol object_class::check_type(const Environment& env)
{
    Environment env_copy = env; //symbol table implementation requires improvement, lookup should be const
    SymEntryData *dat = env_copy.symbol_table.lookup(name);
    if(dat == nullptr)
    {
        std::string errmsg = "object \"" + std::string(name->get_string()) + "\" not declared";
        serror.print_error(get_line_number(), errmsg);
        type = idtable.add_string("Object");
    }
    else
    {
        type = dat->get_typedec();
    }
    
    return type;
}


Symbol Feature_class::check_type(const Environment &env)
{
    attr_class *att = dynamic_cast<attr_class*>(this);
    
    if(att)
    {
        return att->check_type(env);
    }
    else
    {
        method_class *meth = dynamic_cast<method_class*>(this);
        if(meth) 
        {
            return meth->check_type(env);
        }
    }
    return idtable.add_string("_no_type");
}
// interfaces used by Bison
Classes nil_Classes()
{
   return new nil_node<Class_>();
}

Classes single_Classes(Class_ e)
{
   return new single_list_node<Class_>(e);
}

Classes append_Classes(Classes p1, Classes p2)
{
   return new append_node<Class_>(p1, p2);
}

Features nil_Features()
{
   return new nil_node<Feature>();
}

Features single_Features(Feature e)
{
   return new single_list_node<Feature>(e);
}

Features append_Features(Features p1, Features p2)
{
   return new append_node<Feature>(p1, p2);
}

Formals nil_Formals()
{
   return new nil_node<Formal>();
}

Formals single_Formals(Formal e)
{
   return new single_list_node<Formal>(e);
}

Formals append_Formals(Formals p1, Formals p2)
{
   return new append_node<Formal>(p1, p2);
}

Expressions nil_Expressions()
{
   return new nil_node<Expression>();
}

Expressions single_Expressions(Expression e)
{
   return new single_list_node<Expression>(e);
}

Expressions append_Expressions(Expressions p1, Expressions p2)
{
   return new append_node<Expression>(p1, p2);
}

Cases nil_Cases()
{
   return new nil_node<Case>();
}

Cases single_Cases(Case e)
{
   return new single_list_node<Case>(e);
}

Cases append_Cases(Cases p1, Cases p2)
{
   return new append_node<Case>(p1, p2);
}

Program program(Classes classes)
{
  return new program_class(classes);
}

Class_ class_(Symbol name, Symbol parent, Features features, Symbol filename)
{
  return new class__class(name, parent, features, filename);
}

Feature method(Symbol name, Formals formals, Symbol return_type, Expression expr)
{
  return new method_class(name, formals, return_type, expr);
}

Feature attr(Symbol name, Symbol type_decl, Expression init)
{
  return new attr_class(name, type_decl, init);
}

Formal formal(Symbol name, Symbol type_decl)
{
  return new formal_class(name, type_decl);
}

Case branch(Symbol name, Symbol type_decl, Expression expr)
{
  return new branch_class(name, type_decl, expr);
}

Expression assign(Symbol name, Expression expr)
{
  return new assign_class(name, expr);
}

Expression static_dispatch(Expression expr, Symbol type_name, Symbol name, Expressions actual)
{
  return new static_dispatch_class(expr, type_name, name, actual);
}

Expression dispatch(Expression expr, Symbol name, Expressions actual)
{
  return new dispatch_class(expr, name, actual);
}

Expression cond(Expression pred, Expression then_exp, Expression else_exp)
{
  return new cond_class(pred, then_exp, else_exp);
}

Expression loop(Expression pred, Expression body)
{
  return new loop_class(pred, body);
}

Expression typcase(Expression expr, Cases cases)
{
  return new typcase_class(expr, cases);
}

Expression block(Expressions body)
{
  return new block_class(body);
}

Expression let(Symbol identifier, Symbol type_decl, Expression init, Expression body)
{
  return new let_class(identifier, type_decl, init, body);
}

Expression plus(Expression e1, Expression e2)
{
  return new plus_class(e1, e2);
}

Expression sub(Expression e1, Expression e2)
{
  return new sub_class(e1, e2);
}

Expression mul(Expression e1, Expression e2)
{
  return new mul_class(e1, e2);
}

Expression divide(Expression e1, Expression e2)
{
  return new divide_class(e1, e2);
}

Expression neg(Expression e1)
{
  return new neg_class(e1);
}

Expression lt(Expression e1, Expression e2)
{
  return new lt_class(e1, e2);
}

Expression eq(Expression e1, Expression e2)
{
  return new eq_class(e1, e2);
}

Expression leq(Expression e1, Expression e2)
{
  return new leq_class(e1, e2);
}

Expression comp(Expression e1)
{
  return new comp_class(e1);
}

Expression int_const(Symbol token)
{
  return new int_const_class(token);
}

Expression bool_const(Boolean val)
{
  return new bool_const_class(val);
}

Expression string_const(Symbol token)
{
  return new string_const_class(token);
}

Expression new_(Symbol type_name)
{
  return new new__class(type_name);
}

Expression isvoid(Expression e1)
{
  return new isvoid_class(e1);
}

Expression no_expr()
{
  return new no_expr_class();
}

Expression object(Symbol name)
{
  return new object_class(name);
}

