#include "mlir/IR/Builders.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/Verifier.h"

// Conversions

#include "mlir/Conversion/ArithToLLVM/ArithToLLVM.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Pass/Pass.h"

using namespace mlir;

int main() {
  // Create an MLIR context
  MLIRContext context;

  // Load the required dialects
  context.getOrLoadDialect<arith::ArithDialect>();
  context.getOrLoadDialect<func::FuncDialect>();

  // Create a module
  OwningOpRef<ModuleOp> module = ModuleOp::create(UnknownLoc::get(&context));

  // Create a builder to help with constructing operations
  OpBuilder builder(&context);

  // Define the function type (takes no arguments, returns i32)
  auto funcType = builder.getFunctionType({}, builder.getI32Type());

  // Create a function named "main"
  auto funcOp = builder.create<func::FuncOp>(
      builder.getUnknownLoc(), "main", funcType);

  // Add the function to the module
  module->push_back(funcOp);

  // Create a new block inside the function
  Block *entryBlock = funcOp.addEntryBlock();

  // Set the insertion point to the start of the block
  builder.setInsertionPointToStart(entryBlock);

  // Create a constant value of 42
  auto constOp = builder.create<arith::ConstantOp>(
      builder.getUnknownLoc(), builder.getI32Type(),
      builder.getI32IntegerAttr(42));

  // Return the constant value
  builder.create<func::ReturnOp>(builder.getUnknownLoc(), constOp.getResult());

  if(failed(verify(module.get())))
    {
        llvm::errs()<<"Error: Verification Failed\n";
        return 0;
    }



  // Print the MLIR code to stdout
  module->dump();


  // All about Pass
  PassManager pm(&context);
  pm.addPass(createArithToLLVMConversionPass());

  if (mlir::failed(pm.run(*module))) {
    llvm::errs() << "Failed to convert to LLVM dialect\n";
    return 1;
  }


  module->dump();

  return 0;
}
