# cool llvm
A compiler for cool programming language that compiles down to llvm ir.
Based on cool support code [1] and LLVM.

## Build Instructions
```
mkdir build
cd build
cmake ..
make
```

## To run

```
./lexer <source_file_path> | ./parser | ./codegen_semant
```

## Links
[1] https://web.stanford.edu/class/archive/cs/cs143/cs143.1112/materials/handouts/cool-tour.pdf

Stanford MOOC: https://lagunita.stanford.edu/courses/Engineering/Compilers/Fall2014/info

LLVM Language Reference: https://llvm.org/docs/LangRef.html
