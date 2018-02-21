#include "semanterror.h"
extern char *curr_filename;

void SemantError::print_error(int lineno, const std::string &error_msg)
{
    num_errors++;
    std::cerr << curr_filename << ":" << lineno << " \033[31merror\033[0m: " << error_msg << "\n";
}

SemantError::SemantError() : num_errors(0)
{
}
