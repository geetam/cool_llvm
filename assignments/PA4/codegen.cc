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

extern char* curr_filename;
static llvm::LLVMContext llvm_context;
static llvm::IRBuilder<> llvm_ir_builder(llvm_context);
static llvm::Module* llvm_module;

static llvm::Type* llvm_int_type = llvm::Type::getInt32Ty(llvm_context);
std::map <Symbol, llvm::Type*> cool_to_llvm_typemap;


llvm::Type* cool_to_llvm_type(const Symbol coolty)
{
    if(cool_to_llvm_typemap.count(coolty))
        return cool_to_llvm_typemap.find(coolty)->second;
    if(strcmp(coolty->get_string(), "Int") == 0)
    {
        cool_to_llvm_typemap[coolty] = llvm_int_type;
    }
    
    return cool_to_llvm_typemap.find(coolty)->second;
}

llvm::StructType* class__class::get_llvm_type()
{
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
    return structReg;
}

void program_class::init_codegen()
{
    llvm_module = new llvm::Module(curr_filename, llvm_context);
    for(int i = classes->first(); classes->more(i); i = classes->next(i))
    {
        class__class* cls = static_cast<class__class*>(classes->nth(i));
        cool_to_llvm_typemap[cls->getName()] = cls->get_llvm_type();
    }
    
    genIOCode();
}

llvm::Value* program_class::codegen(const Symbol_to_Addr &location_var)
{
    
    llvm::raw_os_ostream cout_raw_os { std::cout };
    for(int i = classes->first(); classes->more(i); i = classes->next(i))
    {
       class__class* cls = static_cast<class__class*>(classes->nth(i));
       cls->codegen(location_var);
    }
    
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
    if(func_name == "Main_main")
    {
        func_name = "main";
    }
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
            mod_location_var.enterscope();
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

llvm::Value* int_const_class::codegen(const Symbol_to_Addr &location_var)
{
    return llvm::ConstantInt::get(llvm_context, llvm::APInt(32, atoi(token->get_string()) , true));
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
    std::string name_s = name->get_string();
    llvm::Function *llvm_func = llvm_module->getFunction(name_s);
    object_class* obj = dynamic_cast <object_class*>(expr);
    
    if(obj)
    {
        std::vector <llvm::Value*> params;
        llvm::Value* loc = tab.lookup(obj->getName());
        llvm::Value *val = llvm_ir_builder.CreateLoad(loc);
        params.push_back(val);
        for(int i = actual->first(); actual->more(i); i = actual->next(i))
        {
            params.push_back(actual->codegen(location_var));
        }
        std::string class_name = obj->getType()->get_string();
        std::string func_name = class_name + '_' + std::string(name->get_string());
        return llvm_ir_builder.CreateCall(llvm_func, params, func_name);
    }
    
}

void class__class::set_attr_llvm()
{
    for(int i = features->first(); features->more(i); i = features->next(i))
    {
        attr_class *attr = dynamic_cast <attr_class*> (features->nth(i));
        if(attr)
        {
            attr_llvm.push_back(attr->getName());
        }
    }
    
    attr_llvm_set = true;
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
        ret = attr_llvm[idx];
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
    std::vector <llvm::Type*> char_ptr(1, llvm::Type::getInt8PtrTy(llvm_context));
    llvm::FunctionType *out_string_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(llvm_context), char_ptr, false);
    llvm::Function *out_string_func = llvm::Function::Create(out_string_type, llvm::Function::ExternalLinkage, "out_string", llvm_module);
    
    llvm::FunctionType *printftype = llvm::FunctionType::get(llvm::Type::getInt32Ty(llvm_context), char_ptr, true);
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(llvm_context, "entry", out_string_func);
    
    llvm_ir_builder.SetInsertPoint(BB);
    llvm::Value *fmt =
        llvm_ir_builder.CreateGlobalStringPtr("The sum is: %s\n");
    

    llvm::Value *out_string_ptr = llvm_module->getOrInsertFunction("out_string", out_string_type);
    llvm::Value *printf_ptr = llvm_module->getOrInsertFunction("printf", printftype);

    std::vector <llvm::Value *> Args;
    Args.push_back(out_string_func->args().begin());
    llvm::Value *ret = llvm_ir_builder.CreateCall(printf_ptr, Args, "calltmp");

    return llvm_ir_builder.CreateRet(ret);

}
