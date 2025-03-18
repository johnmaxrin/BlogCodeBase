# Generating and Lowering Structs in MLIR to LLVM IR
This repository demonstrates how to define and emit structured types (structs) in MLIR, lower them to LLVM IR, and generate an executable binary. The example covers creating an MLIR structure, allocating it, and compiling it into LLVM IR.

### Build and Compilation Steps
- Set Up the MLIR Context
Ensure that Clang and OpenMP are installed and configured.

- Generate LLVM IR from MLIR
```sh
mlir-translate --mlir-to-llvmir output.mlir -o output.ll
```

- Compile with Clang
```sh
clang output.ll -o a.out -fopenmp
```

- Run the Executable
```sh
./a.out
```

