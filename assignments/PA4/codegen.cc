#include "cool-tree.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"
#include <map>
#include <vector>
#include "symtab.h"
#include <llvm/Support/raw_os_ostream.h>
#include "basicclasses.h"

extern char* curr_filename;
static llvm::LLVMContext llvm_context;
static llvm::IRBuilder<> llvm_ir_builder(llvm_context);
static llvm::Module* llvm_module;
static llvm::Type* llvm_int_type = llvm::Type::getInt32Ty(llvm_context);
BasicClasses basic_classes;
std::map <Symbol, llvm::Type*> cool_to_llvm_typemap;


llvm::Type* cool_to_llvm_type(const Symbol coolty)
{
    if(cool_to_llvm_typemap.count(coolty))
        return cool_to_llvm_typemap.find(coolty)->second;
    if(strcmp(coolty->get_string(), "Int") == 0)
    {
        cool_to_llvm_typemap[coolty] = llvm_int_type;
    }
    else if(strcmp(coolty->get_string(), "Bool") == 0)
    {
        cool_to_llvm_typemap[coolty] = llvm::Type::getInt8Ty(llvm_context);
    }
    else if(strcmp(coolty->get_string(), "IO") == 0)
    {
        cool_to_llvm_typemap[coolty] = basic_classes.get_IO_class()->get_llvm_type();
    }

    return cool_to_llvm_typemap.find(coolty) != cool_to_llvm_typemap.end()?
           cool_to_llvm_typemap.find(coolty)->second : nullptr;
}

llvm::StructType* class__class::get_llvm_type()
{
    if(llvm_type_set)
        return llvm_type;
    std::vector <llvm::Type*> tyvec;
    for(int i = features->first(); features->more(i); i = features->next(i))
    {
        attr_class* att = dynamic_cast <attr_class*>(features->nth(i));
        if(att)
        {
            tyvec.push_back(cool_to_llvm_type(att->getTypeDec()));
        }
    }
    std::string name_of_type = "class." + std::string(name->get_string());
    llvm::StructType *structReg = llvm::StructType::create(llvm_context,tyvec, name_of_type);
    llvm_type_set = true;
    llvm_type = structReg;
    return llvm_type;
}

void program_class::init_codegen(const Environment &semant_env)
{
    
    llvm_module = new llvm::Module(curr_filename, llvm_context);
    for(int i = classes->first(); classes->more(i); i = classes->next(i))
    {
        class__class* cls = static_cast<class__class*>(classes->nth(i));
        cool_to_llvm_typemap[cls->getName()] = cls->get_llvm_type();
    }
}

llvm::Value* program_class::codegen(const Symbol_to_Addr &location_var)
{
    llvm::raw_os_ostream cout_raw_os { std::cout };
    
    genIOCode();
    for(int i = classes->first(); classes->more(i); i = classes->next(i))
    {
       class__class* cls = static_cast<class__class*>(classes->nth(i));
       cls->gen_constructor();
       cls->codegen(location_var);
    }
    gen_main();
    llvm_module->print(cout_raw_os, nullptr);
    
}

llvm::Value* class__class::codegen(const Symbol_to_Addr &location_var)
{
    
    for(int i = features->first(); features->more(i); i = features->next(i))
    {
        method_class *meth = dynamic_cast <method_class*> (features->nth(i));
        if(meth)
        {
            meth->codegen(location_var, this);
        }
    }
}

llvm::Value* method_class::codegen(const Symbol_to_Addr &location_var, class__class *cls)
{
    llvm::Type* llvm_return_type = cool_to_llvm_type(return_type);
    std::vector <llvm::Type*> llvm_params;
    auto class_type = cool_to_llvm_type(cls->getName());
    llvm_params.push_back(llvm::PointerType::get(class_type, 0));
    for(int i = formals->first(); formals->more(i); i = formals->next(i))
    {
        formal_class* formalptr = static_cast <formal_class*>(formals->nth(i));
        llvm_params.push_back(cool_to_llvm_type(formalptr->getTypeDec()));
    }
    
    std::string func_name = std::string(cls->getName()->get_string()) + '_' + std::string(name->get_string());
    llvm::FunctionType* llvm_func_type = llvm::FunctionType::get(llvm_return_type, llvm_params, false);
    llvm::Function *llvm_func = llvm::Function::Create(llvm_func_type, llvm::Function::ExternalLinkage,
                                                       func_name, llvm_module);


    llvm::BasicBlock *bablk = llvm::BasicBlock::Create(llvm_context, "", llvm_func);
    llvm_ir_builder.SetInsertPoint(bablk);
    
    for (auto &arg : llvm_func->args())
    {
        llvm::AllocaInst *Alloca = llvm_ir_builder.CreateAlloca(arg.getType(), 0, arg.getName());
        llvm_ir_builder.CreateStore(&arg, Alloca);
    }
    
    Symbol_to_Addr mod_location_var = location_var;
    mod_location_var.enterscope();
    llvm::StructType *struct_type = static_cast<llvm::StructType*> (class_type);
    int num_attrs_class = struct_type->getNumElements();
    auto class_attrs_ptr = llvm_ir_builder.CreateLoad(llvm_func->args().begin(), "");
    
    for(int i = 0; i < num_attrs_class; i++)
    {
        std::vector<llvm::Value*> indices(2);
        auto zero_int = llvm::ConstantInt::get(llvm_context, llvm::APInt(32, 0, true));
        indices[0] = zero_int;
        indices[1] = llvm::ConstantInt::get(llvm_context, llvm::APInt(32, i, true));
        llvm::Value* attr_ptr = llvm_ir_builder.CreateGEP(class_attrs_ptr->getType(), llvm_func->args().begin(), indices, "memberptr");
        //auto loaded_member = llvm_ir_builder.CreateLoad(attr_ptr, "loadtmp");
        auto attr = cls->attr_at_index(i);
        if(attr.first)
        {
            mod_location_var.addid(attr.second, attr_ptr);
        }
        else
        {
            std:cerr << "No such att\n";
        }
        
    }
    
    llvm::Value* expr_code = expr->codegen(mod_location_var);
    if(expr_code)
    {
        return llvm_ir_builder.CreateRet(expr_code);
    }
    else
    {
        llvm_func->eraseFromParent();
        return nullptr;
    }
    
}

llvm::Value* plus_class::codegen(const Symbol_to_Addr &location_var)
{
    llvm::Value* left_val = e1->codegen(location_var);
    llvm::Value* right_val = e2->codegen(location_var);
    
    llvm::Value *sum = llvm_ir_builder.CreateAdd(left_val, right_val, "plus_class");
    return sum;
}

llvm::Value* sub_class::codegen(const Symbol_to_Addr &location_var)
{
    llvm::Value* left_val = e1->codegen(location_var);
    llvm::Value* right_val = e2->codegen(location_var);
    
    llvm::Value *diff = llvm_ir_builder.CreateSub(left_val, right_val, "sub_class");
    return diff;
}

llvm::Value* mul_class::codegen(const Symbol_to_Addr &location_var)
{
    llvm::Value* left_val = e1->codegen(location_var);
    llvm::Value* right_val = e2->codegen(location_var);
    
    llvm::Value *mul = llvm_ir_builder.CreateMul(left_val, right_val, "mul_class");
    return mul;
}

llvm::Value* divide_class::codegen(const Symbol_to_Addr &location_var)
{
    llvm::Value* left_val = e1->codegen(location_var);
    llvm::Value* right_val = e2->codegen(location_var);
    
    llvm::Value *div = llvm_ir_builder.CreateSDiv(left_val, right_val, "divide_class");
    return div;
}

llvm::Value* int_const_class::codegen(const Symbol_to_Addr &location_var)
{
    return llvm::ConstantInt::get(llvm_context, llvm::APInt(32, atoi(token->get_string()) , true));
}

llvm::Value* string_const_class::codegen(const Symbol_to_Addr &location_var)
{
    return llvm_ir_builder.CreateGlobalStringPtr(token->get_string());
}

llvm::Value* object_class::codegen(const Symbol_to_Addr &location_var)
{
    Symbol_to_Addr tab = location_var;
    llvm::Value* loc = tab.lookup(name);
    return llvm_ir_builder.CreateLoad(loc);
}

llvm::Value* dispatch_class::codegen(const Symbol_to_Addr &location_var)
{
    Symbol_to_Addr tab = location_var;
    object_class* obj = dynamic_cast <object_class*>(expr);

    if(obj)
    {
        std::vector <llvm::Value*> params;
        llvm::Value* loc = tab.lookup(obj->getName());
        llvm::Value *val = llvm_ir_builder.CreateLoad(loc);
        params.push_back(loc);
        for(int i = actual->first(); actual->more(i); i = actual->next(i))
        {
            params.push_back(actual->nth(i)->codegen(location_var));
        }
        std::string class_name = obj->getType()->get_string();
        std::string func_name = class_name + '_' + std::string(name->get_string());
        llvm::Function *llvm_func = llvm_module->getFunction(func_name);
        if(llvm_func)
        {
            return llvm_ir_builder.CreateCall(llvm_func, params);
        }
    }
    return nullptr;
}

void class__class::set_attr_llvm()
{
    if(!attr_llvm_set)
    {
        for(int i = features->first(); features->more(i); i = features->next(i))
        {
            attr_class *attr = dynamic_cast <attr_class*> (features->nth(i));
            if(attr)
            {
                attr_llvm.push_back(attr);
            }
        }
        attr_llvm_set = true;
    }
}

std::pair <bool, Symbol> class__class::attr_at_index(int idx)
{
    if(!attr_llvm_set)
    {
        set_attr_llvm();
    }
    bool valid = true;
    Symbol ret;
    try
    {
        ret = attr_llvm[idx]->getName();
    }
    catch(std::out_of_range e)
    {
        ret = idtable.add_string("Not Found");
        valid = false;
    }
    
    return std::make_pair(valid, ret);
    
}

llvm::Value* program_class::genIOCode()
{
    Symbol IO = idtable.add_string("IO");
    llvm::PointerType* pointer_to_IO_class = llvm::PointerType::get(cool_to_llvm_type(IO), 0);
    std::vector <llvm::Type*> out_string_params = { pointer_to_IO_class, llvm::Type::getInt8PtrTy(llvm_context) };
    llvm::FunctionType *out_string_type = llvm::FunctionType::get(cool_to_llvm_type(IO), out_string_params, false);
    llvm::Function *out_string_func = llvm::Function::Create(out_string_type, llvm::Function::ExternalLinkage, "IO_out_string", llvm_module);
    std::vector <llvm::Type*> printfargs(1, llvm::Type::getInt8PtrTy(llvm_context));
    llvm::FunctionType *printftype = llvm::FunctionType::get(llvm::Type::getInt32Ty(llvm_context), printfargs, true);
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(llvm_context, "entry", out_string_func);
    
    llvm_ir_builder.SetInsertPoint(BB);
    llvm::Value *printf_ptr = llvm_module->getOrInsertFunction("printf", printftype);

    std::vector <llvm::Value *> Args;
    Args.push_back(out_string_func->args().begin() + 1);
    llvm::Value *ret = llvm_ir_builder.CreateCall(printf_ptr, Args, "calltmp");
    llvm::Value* io_val = llvm_ir_builder.CreateLoad(out_string_func->args().begin());
    llvm_ir_builder.CreateRet(io_val);
    
    //define out_int
    std::vector <llvm::Type*> out_int_params = { pointer_to_IO_class, llvm::Type::getInt32Ty(llvm_context) };
    llvm::FunctionType *out_int_type = llvm::FunctionType::get(cool_to_llvm_type(IO), out_int_params, false);
    llvm::Function *out_int_func = llvm::Function::Create(out_int_type, llvm::Function::ExternalLinkage, "IO_out_int"
                                    , llvm_module);
    llvm::BasicBlock *bb_out_int = llvm::BasicBlock::Create(llvm_context, "entry", out_int_func);
    llvm_ir_builder.SetInsertPoint(bb_out_int);
    std::vector <llvm::Value *> Args_out_int;
    llvm::Value *fmt = llvm_ir_builder.CreateGlobalStringPtr("%d\n", "out_int_fmt");
    Args_out_int.push_back(fmt);
    Args_out_int.push_back(out_int_func->args().begin() + 1);
    llvm_ir_builder.CreateCall(printf_ptr, Args_out_int, "call_out_int");    
    llvm::Value* io_val_out_int = llvm_ir_builder.CreateLoad(out_int_func->args().begin());
    return llvm_ir_builder.CreateRet(io_val_out_int);

}

void class__class::gen_constructor()
{
    llvm::StructType* cls_type = get_llvm_type();
    llvm::PointerType* pointer_to_cls_type = llvm::PointerType::get(cls_type, 0);
    std::string class_name = name->get_string();
    std::string func_name = class_name + '_' + class_name;
    std::vector <llvm::Type*> params(1, pointer_to_cls_type);
    llvm::FunctionType* llvm_func_type = llvm::FunctionType::get(llvm::Type::getVoidTy(llvm_context), params, false);
    llvm::Function *llvm_func = llvm::Function::Create(llvm_func_type, llvm::Function::ExternalLinkage,
                                                       func_name, llvm_module);

    llvm::BasicBlock *bablk = llvm::BasicBlock::Create(llvm_context, "constructor_func", llvm_func);
    llvm_ir_builder.SetInsertPoint(bablk);
    //copy the passed argument
    llvm::AllocaInst *ptr_to_ptr = llvm_ir_builder.CreateAlloca(pointer_to_cls_type);
    llvm::Value* ptr_to_obj = llvm_func->args().begin();
    llvm::Value* str_of_ptr = llvm_ir_builder.CreateStore(ptr_to_obj, ptr_to_ptr);
    llvm::Value* copied_ptr_to_obj = llvm_ir_builder.CreateLoad(pointer_to_cls_type, ptr_to_ptr);
    
    
    set_attr_llvm();
    int num_attrs_class = cls_type->getNumElements();
    Symbol_to_Addr location_var;
    location_var.enterscope();
    for(int i = 0; i < num_attrs_class; i++)
    {
        std::vector<llvm::Value*> indices(2);
        auto zero_int = llvm::ConstantInt::get(llvm_context, llvm::APInt(32, 0, true));
        indices[0] = zero_int;
        indices[1] = llvm::ConstantInt::get(llvm_context, llvm::APInt(32, i, true));
        std::string name_gep = std::string(attr_llvm[i]->getName()->get_string()) + "_ptr" ;
        llvm::Value* attr_ptr = llvm_ir_builder.CreateGEP(cls_type, copied_ptr_to_obj, indices, name_gep);
        location_var.addid(attr_llvm[i]->getName(), attr_ptr);
    }
    for(int i = 0; i < num_attrs_class; i++)
    {
        no_expr_class* init_no_expr = dynamic_cast <no_expr_class*> (attr_llvm[i]->getInit());
        if(init_no_expr == nullptr)
        {
            llvm::Value* expr_code = attr_llvm[i]->getInit()->codegen(location_var);
            llvm_ir_builder.CreateStore(expr_code, location_var.lookup(attr_llvm[i]->getName()));
        }
        //TODO: else { call constructor of att }
   
    }
    llvm_ir_builder.CreateRet(nullptr);
}

llvm::Function* program_class::gen_main()
{
    llvm::FunctionType *maintype = llvm::FunctionType::get(llvm::Type::getInt32Ty(llvm_context), false);
    llvm::Function *main_func = llvm::Function::Create(maintype, llvm::Function::ExternalLinkage, "main", llvm_module);
    llvm::BasicBlock *main_entry = llvm::BasicBlock::Create(llvm_context, "main_entry", main_func);
    llvm_ir_builder.SetInsertPoint(main_entry);
    llvm::StructType* Main_type = llvm_module->getTypeByName("class.Main");
    if(Main_type)
    {
        //Main_Main is the constructor, Main_main is the defined main func inside Main
        llvm::AllocaInst* aloc = llvm_ir_builder.CreateAlloca(Main_type);
        std::vector <llvm::Value*> Args;
        Args.push_back ( aloc );
        
        llvm::Value* Main_Main = llvm_module->getFunction("Main_Main");
        if(Main_Main)
        {
            llvm_ir_builder.CreateCall(Main_Main, Args);
        }
        //now call main() defined by programmer inside Main
        llvm::Value *Main_main = llvm_module->getFunction("Main_main");
        if(Main_main)
        {
            llvm_ir_builder.CreateCall(Main_main, Args);
        }
        
        llvm_ir_builder.CreateRet(llvm::ConstantInt::get(llvm_context, llvm::APInt(32, 0, true)));
        return main_func;
    }
    return nullptr;
}

llvm::Value* block_class::codegen(const Symbol_to_Addr &location_var)
{
    llvm::Value* val;
    for(int i = body->first(); body->more(i);  i = body->next(i))
    {
        val = body->nth(i)->codegen(location_var);
    }
    return val;
}
