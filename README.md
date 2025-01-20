# Arith to LLVM Conversion
This branch contains the code and resources for the blog post on [Converting Arith to LLVM using MLIR](https://medium.com/@60b36t/converting-arith-dialect-to-llvm-dialect-in-mlir-8b393615b54d). The blog explores how to implement a simple conversion pass in 
MLIR to lower operations from the arith dialect to the llvm dialect, enabling efficient code generation for LLVM-based backends.

## Building the Project
Clone the repository and switch to this branch:

```sh
git clone https://github.com/johnmaxrin/BlogCodeBase.git
cd BlogCodeBase
git checkout arith2llvm
```

Build the project using CMake:
```sh
mkdir build && cd build
cmake .. && make
```

## How to Run
```sh
cd build/bin
./app
```
