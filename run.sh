./bin/app
mlir-translate --mlir-to-llvmir op.mlir -o op3.ll
llc -filetype=obj -relocation-model=pic op3.ll -o op.o
mpicc op.o -L/usr/lib/llvm-19/lib -lomp
mpirun -np 3   ./a.out 