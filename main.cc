#include "mlir/IR/Builders.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/Verifier.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/Dialect/OpenMP/OpenMPDialect.h"

// Conversions

#include "mlir/Conversion/ArithToLLVM/ArithToLLVM.h"
#include "mlir/Conversion/OpenMPToLLVM/ConvertOpenMPToLLVM.h"

#include "mlir/Pass/PassManager.h"
#include "mlir/Pass/Pass.h"

using namespace mlir;

int main()
{
  // Create an MLIR context
  MLIRContext context;

  // Load the required dialects
  context.getOrLoadDialect<mlir::omp::OpenMPDialect>();
  context.getOrLoadDialect<mlir::LLVM::LLVMDialect>();

  mlir::OpBuilder builder(&context);
  mlir::ModuleOp module = builder.create<mlir::ModuleOp>(builder.getUnknownLoc());



  auto ukwnloc = builder.getUnknownLoc();

  // Createa a strucutural type



  auto int32Ty = builder.getI32Type();

  auto funcType = LLVM::LLVMFunctionType::get(int32Ty, {int32Ty});

  auto funcOp = builder.create<mlir::LLVM::LLVMFuncOp>(ukwnloc, "main", funcType);
  module.push_back(funcOp);

  auto block = funcOp.addEntryBlock(builder);

  builder.setInsertionPointToStart(block);

  auto structType = LLVM::LLVMStructType::getIdentified(&context, "Graph");
  structType.setBody({builder.getI32Type()},false);

  Type structPtrType = LLVM::LLVMPointerType::get(&context);


  auto one = builder.create<LLVM::ConstantOp>(ukwnloc, builder.getI64Type(), builder.getI64IntegerAttr(1));


  auto graphDecl = builder.create<mlir::LLVM::AllocaOp>(ukwnloc, structPtrType, structType, one);


  auto constOp = builder.create<mlir::LLVM::ConstantOp>(ukwnloc, int32Ty, builder.getI32IntegerAttr(33));
  auto retOp = builder.create<mlir::LLVM::ReturnOp>(ukwnloc, constOp->getResult(0));

  if (failed(verify(module)))
  {
    llvm::errs() << "Error: Verification Failed\n";
    return 0;
  }

  PassManager pm(&context);
  pm.addPass(createConvertOpenMPToLLVMPass());
  if (failed(pm.run(module)))
  {
    llvm::errs() << "Failed to run passes\n";
    return 1;
  }
  // Print the MLIR code to stdout
  std::error_code error;
  llvm::raw_fd_ostream outputFile("output.mlir", error);
  module.print(outputFile);
  outputFile.close();

  return 0;
}
