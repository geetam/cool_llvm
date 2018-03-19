#include <stdio.h>
#include "cool-tree.h"
#include "semanterror.h"
#include <llvm/Support/raw_os_ostream.h>
#include "llvm/IR/Module.h"


extern Program ast_root;      // root of the abstract syntax tree
FILE *ast_file = stdin;       // we read the AST from standard input
extern int ast_yyparse(void); // entry point to the AST parser

int cool_yydebug;     // not used, but needed to link with handle_flags
char *curr_filename;
SemantError serror;

void handle_flags(int argc, char *argv[]);

int main(int argc, char *argv[]) {
  handle_flags(argc,argv);
  ast_yyparse();
  ast_root->semant();
  program_class *ast_root_p = static_cast<program_class*>(ast_root);
  ast_root_p->init_codegen();
  Symbol_to_Addr location_var;
  ast_root_p->codegen(location_var);
  //ast_root->dump_with_types(cout,0);
}

