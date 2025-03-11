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

  auto i8Type = builder.getI8Type();
  auto i8PtrType = mlir::LLVM::LLVMPointerType::get(&context);

  const char *str = "Hello World\n\0";
  size_t len = strlen(str);

  auto arrayType = mlir::LLVM::LLVMArrayType::get(i8Type, len);

  auto globalStr = builder.create<mlir::LLVM::GlobalOp>(ukwnloc, arrayType, true, mlir::LLVM::Linkage::Internal, ".str", builder.getStringAttr(str));

  module.push_back(globalStr);

  // Define Printf
  auto printfType = mlir::LLVM::LLVMFunctionType::get(i8PtrType, {i8PtrType}, true);
  auto printfFunc = builder.create<mlir::LLVM::LLVMFuncOp>(ukwnloc, "printf", printfType);

  module.push_back(printfFunc);

  auto int32Ty = builder.getI32Type();

  auto funcType = LLVM::LLVMFunctionType::get(int32Ty, {int32Ty});

  auto funcOp = builder.create<mlir::LLVM::LLVMFuncOp>(ukwnloc, "main", funcType);
  module.push_back(funcOp);

  auto block = funcOp.addEntryBlock(builder);

  builder.setInsertionPointToStart(block);

  auto zero = builder.create<mlir::LLVM::ConstantOp>(ukwnloc, int32Ty, builder.getI32IntegerAttr(0));
  auto one = builder.create<mlir::LLVM::ConstantOp>(ukwnloc, int32Ty, builder.getI32IntegerAttr(1));
  auto strPtr = builder.create<mlir::LLVM::AddressOfOp>(ukwnloc, globalStr);

  auto gep = builder.create<mlir::LLVM::GEPOp>(ukwnloc, i8PtrType, arrayType, strPtr, mlir::ValueRange{zero, zero});

 
  mlir::Location dummyLoc = mlir::FileLineColLoc::get(builder.getStringAttr("dummy.mlir"), 0, 0);
  auto parallelOp = builder.create<mlir::omp::ParallelOp>(dummyLoc);
  auto &parallelRegion = parallelOp.getRegion();
  auto *parallelBlock = builder.createBlock(&parallelRegion);
  builder.setInsertionPointToStart(parallelBlock);

  // USE GEP
  // auto gep = builder.create<mlir::LLVM::GEPOp>(ukwnloc, i8PtrType, arrayType, strPtr, mlir::ValueRange{zero,zero});
  builder.create<mlir::LLVM::CallOp>(ukwnloc, printfFunc, mlir::ValueRange{gep});
  builder.create<mlir::omp::TerminatorOp>(ukwnloc);
  builder.setInsertionPointAfter(parallelOp);

  // Return 33
  // Creare 33
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
  module->dump();

  return 0;
}
