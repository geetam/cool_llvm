#ifndef SEMANTERROR_H
#define SEMANTERROR_H

#include <iostream>

class SemantError {
private:
  int num_errors;

public:
  int get_num_errors() { return num_errors; }
  void print_error(int lineno, const std::string &error_msg);
};


#endif
