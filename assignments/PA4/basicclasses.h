#ifndef BASICCLASSES_H
#define BASICCLASSES_H

#include "stringtab.h"
#include "cool-tree.h"

//make_x_class() from semant.cc
class BasicClasses {
private:
    Symbol arg, arg2, Bool, concat, cool_abort,
           copy, Int, in_int, in_string, IO, length, Main,
           main_meth, No_class, No_type, Object, out_int,
           out_string, prim_slot, self, SELF_TYPE, Str,
           str_field,substr, type_name, val;
    class__class *Object_class, *IO_class, *String_class,
                 *Int_class, *Bool_class;

    class__class* make_Object_class()
    {
        return static_cast<class__class*>(
                   class_(Object,
                          No_class,
                          append_Features(
                              append_Features(
                                  single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
                                  single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
                              single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
                          stringtable.add_string("<basic class>")));
    }

    class__class* make_IO_class()
    {
        return static_cast<class__class*>(
                   class_(IO,
                          Object,
                          append_Features(
                              append_Features(
                                  append_Features(
                                      single_Features(method(out_string, single_Formals(formal(arg, Str)),
                                              SELF_TYPE, no_expr())),
                                      single_Features(method(out_int, single_Formals(formal(arg, Int)),
                                              SELF_TYPE, no_expr()))),
                                  single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
                              single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
                          stringtable.add_string("<basic class>")));

    }
    class__class* make_Int_class()
    {
        return static_cast<class__class*>(
                   class_(Int,
                          Object,
                          single_Features(attr(val, prim_slot, no_expr())),
                          stringtable.add_string("<basic class>")));
    }

    class__class* make_Bool_class()
    {
        return static_cast<class__class*>(
                   class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),stringtable.add_string("<basic class>")));
    }

    class__class* make_String_class()
    {
        return static_cast<class__class*>(
                   class_(Str,
                          Object,
                          append_Features(
                              append_Features(
                                  append_Features(
                                      append_Features(
                                          single_Features(attr(val, Int, no_expr())),
                                          single_Features(attr(str_field, prim_slot, no_expr()))),
                                      single_Features(method(length, nil_Formals(), Int, no_expr()))),
                                  single_Features(method(idtable.add_string("concat"),
                                          single_Formals(formal(arg, Str)),
                                          Str,
                                          no_expr()))),
                              single_Features(method(substr,
                                              append_Formals(single_Formals(formal(arg, Int)),
                                                      single_Formals(formal(arg2, Int))),
                                              Str,
                                              no_expr()))),
                          stringtable.add_string("<basic class>")));
    }

public:

    BasicClasses() : Object_class(nullptr), IO_class(nullptr), String_class(nullptr),
        Int_class(nullptr), Bool_class(nullptr)
    {
        arg         = idtable.add_string("arg");
        arg2        = idtable.add_string("arg2");
        Bool        = idtable.add_string("Bool");
        cool_abort  = idtable.add_string("abort");
        copy        = idtable.add_string("copy");
        Int         = idtable.add_string("Int");
        in_int      = idtable.add_string("in_int");
        in_string   = idtable.add_string("in_string");
        IO          = idtable.add_string("IO");
        length      = idtable.add_string("length");
        Main        = idtable.add_string("Main");
        main_meth   = idtable.add_string("main");
        No_class    = idtable.add_string("_no_class");
        No_type     = idtable.add_string("_no_type");
        Object      = idtable.add_string("Object");
        out_int     = idtable.add_string("out_int");
        out_string  = idtable.add_string("out_string");
        prim_slot   = idtable.add_string("_prim_slot");
        self        = idtable.add_string("self");
        SELF_TYPE   = idtable.add_string("SELF_TYPE");
        Str         = idtable.add_string("String");
        str_field   = idtable.add_string("_str_field");
        substr      = idtable.add_string("substr");
        type_name   = idtable.add_string("type_name");
        val         = idtable.add_string("_val");
    }
    
    class__class *get_Object_class()
    {
        if(Object_class == nullptr)
        {
            Object_class = make_Object_class();
        }
        
        return Object_class;
    }
    
        
    class__class *get_Int_class()
    {
        if(Int_class == nullptr)
        {
           Int_class = make_Int_class();
        }
        
        return Int_class;
    }
        
    class__class *get_String_class()
    {
        if(String_class == nullptr)
        {
            String_class = make_String_class();
        }
        
        return String_class;
    }
    
        
    class__class *get_Bool_class()
    {
        if(Bool_class == nullptr)
        {
            Bool_class = make_Bool_class();
        }
        
        return Bool_class;
    }
    
        
    class__class *get_IO_class()
    {
        if(IO_class == nullptr)
        {
            IO_class = make_IO_class();
        }
        
        return IO_class;
    }
};
#endif
