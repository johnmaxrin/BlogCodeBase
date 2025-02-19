# MLIR Symbol Table Primer

This repository contains the code and resources for the blog post ["A Primer on MLIR's Symbol Table"](https://medium.com/@60b36t/a-primer-on-mlirs-symbol-table-996a0bc5728f). It explores how to use MLIR's symbol infrastructure, including:

- Defining symbols in a custom dialect
- Managing named operations using mlir::SymbolTable
- Looking up symbols efficiently in MLIR
    
## Building the Project
```sh
git clone https://github.com/johnmaxrin/BlogCodeBase.git
cd BlogCodeBase
git checkout symboltableprimer
```

## Build the project using CMake:
```sh
mkdir build && cd build
cmake .. && make
```

## How to Run
```sh
cd build/bin
./app
```
