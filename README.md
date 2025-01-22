# Custom Dialect and Pass
This branch contains the code and resources for the blog post on [Understanding MLIR Passes Through a Simple Dialect Transformation](https://medium.com/@60b36t/understanding-mlir-passes-through-a-simple-dialect-transformation-879ca47f504f).
The blog explores how to implement a simple custom conversion pass in 
MLIR to lower operations from the custom dialect to arith dialect.

## Building the Project
```sh
git clone https://github.com/johnmaxrin/BlogCodeBase.git
cd BlogCodeBase
git checkout convertmydialect2arith
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
