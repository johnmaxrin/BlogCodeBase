# Emitting OpenMP Code from MLIR and Lowering to LLVM IR
This repository demonstrates how to emit OpenMP (omp) code using the MLIR OpenMP dialect, lower it to LLVM IR, and generate an executable binary.

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

- Expected Output
The program prints "Hello World" multiple times, depending on the number of OpenMP threads available.