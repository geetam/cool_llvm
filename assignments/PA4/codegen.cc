#include "cool-tree.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"
#include <map>
#include <vector>

extern char* curr_filename;
static llvm::LLVMContext llvm_context;
static llvm::IRBuilder<> llvm_ir_builder(llvm_context);
static llvm::Module* llvm_module;
static std::map<std::string, llvm::Value *> NamedValues;

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
void program_class::init_codegen()
{
    llvm_module = new llvm::Module(curr_filename, llvm_context);
}

llvm::Value* program_class::codegen()
{ 
    for(int i = classes->first(); classes->more(i); i = classes->next(i))
    {
       class__class* cls = static_cast<class__class*>(classes->nth(i));
       cls->codegen();
    }
    
    llvm_module->print(llvm::errs(), nullptr);
    
}

llvm::Value* class__class::codegen()
{
    for(int i = features->first(); features->more(i); i = features->next(i))
    {
        method_class *meth = dynamic_cast <method_class*> (features->nth(i));
        if(meth)
        {
            meth->codegen();
        }
    }
}

llvm::Value* method_class::codegen()
{
    llvm::Type* llvm_return_type = cool_to_llvm_type(return_type);
    std::vector <llvm::Type*> llvm_params;
    for(int i = formals->first(); formals->more(i); i = formals->next(i))
    {
        formal_class* formalptr = static_cast <formal_class*>(formals->nth(i));
        llvm_params.push_back(cool_to_llvm_type(formalptr->getTypeDec()));
    }
    
    
    llvm::FunctionType* llvm_func_type = llvm::FunctionType::get(llvm_return_type, llvm_params, false);
    llvm::Function *llvm_func = llvm::Function::Create(llvm_func_type, llvm::Function::ExternalLinkage,
                                                       name->get_string(), llvm_module);
    

    llvm::BasicBlock *bablk = llvm::BasicBlock::Create(llvm_context, "", llvm_func);
    
    llvm_ir_builder.SetInsertPoint(bablk);
    llvm::Value* expr_code = expr->codegen();
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

llvm::Value* plus_class::codegen()
{
    llvm::Value* left_val = e1->codegen();
    llvm::Value* right_val = e2->codegen();
    
    llvm::Value *sum = llvm_ir_builder.CreateAdd(left_val, right_val, "plus_class");
}

llvm::Value* int_const_class::codegen()
{
    return llvm::ConstantInt::get(llvm_context, llvm::APInt(32, atoi(token->get_string()) , true));
}
