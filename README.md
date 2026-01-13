### Translate to LLVM IR
```
mlir-translate sample.mlir --mlir-to-llvmir -o sample.ll
```

### Compile 
```
clang++ runtime.cc sample.ll -O3 -lcuda -o run_kernel
```

### Run
```
./run_kernel
```
