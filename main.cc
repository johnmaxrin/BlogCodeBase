#include "mlir/IR/Builders.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Transforms/DialectConversion.h"
#include "mlir/Conversion/LLVMCommon/TypeConverter.h"   
#include "mlir/Conversion/Passes.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/ControlFlow/IR/ControlFlow.h"
#include "mlir/Dialect/GPU/IR/GPUDialect.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/Dialect/LLVMIR/NVVMDialect.h"
#include "mlir/Dialect/GPU/Transforms/Passes.h"
#include "mlir/Dialect/LLVMIR/NVVMDialect.h"
#include "mlir/Target/LLVM/NVVM/Target.h"

#include "mlir/IR/Verifier.h"
// Conversions
#include "mlir/Conversion/ArithToLLVM/ArithToLLVM.h"
#include "mlir/Conversion/GPUToNVVM/GPUToNVVMPass.h"
#include "mlir/Conversion/GPUCommon/GPUCommonPass.h"
#include "mlir/Dialect/GPU/Transforms/Passes.h"
#include "mlir/Conversion/IndexToLLVM/IndexToLLVM.h"
#include "mlir/Conversion/FuncToLLVM/ConvertFuncToLLVMPass.h"
#include "mlir/Conversion/FuncToLLVM/ConvertFuncToLLVM.h"
#include "mlir/Conversion/UBToLLVM/UBToLLVM.h"
#include "mlir/Conversion/ControlFlowToLLVM/ControlFlowToLLVM.h"
#include "mlir/Conversion/ReconcileUnrealizedCasts/ReconcileUnrealizedCasts.h"
#include "mlir/Conversion/GPUCommon/GPUCommonPass.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Pass/Pass.h"

#include "mlir/Target/LLVM/NVVM/Target.h"
#include "mlir/Target/LLVMIR/Dialect/Builtin/BuiltinToLLVMIRTranslation.h"
#include "mlir/Target/LLVMIR/Dialect/NVVM/NVVMToLLVMIRTranslation.h"
#include "mlir/Target/LLVMIR/Dialect/LLVMIR/LLVMToLLVMIRTranslation.h"
#include "mlir/Target/LLVMIR/Dialect/GPU/GPUToLLVMIRTranslation.h"


// Extension registration headers
#include "mlir/Dialect/Func/Extensions/AllExtensions.h"

using namespace mlir;

int main() {
  // Create an MLIR context and enable multi-threading
  MLIRContext context(MLIRContext::Threading::DISABLED);
  
  // Register dialects FIRST before creating any operations
  context.loadDialect<arith::ArithDialect>();
  context.loadDialect<func::FuncDialect>();
  context.loadDialect<gpu::GPUDialect>();
  context.loadDialect<LLVM::LLVMDialect>();
  context.loadDialect<NVVM::NVVMDialect>();
  
  // Register only the func extensions in a registry and apply it
  DialectRegistry registry;
  registerConvertNVVMToLLVMInterface(registry);
  
  registerConvertFuncToLLVMInterface(registry);
  cf::registerConvertControlFlowToLLVMInterface(registry);
  arith::registerConvertArithToLLVMInterface(registry);
  registerConvertMemRefToLLVMInterface(registry);
  NVVM::registerNVVMTargetInterfaceExternalModels(registry);
  
  func::registerAllExtensions(registry);
  context.appendDialectRegistry(registry);
  mlir::registerNVVMDialectTranslation(registry);
  mlir::registerLLVMDialectTranslation(registry);
  mlir::registerGPUDialectTranslation(registry);
  
  
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
  
  // Create a constant value of 1
  auto constoneOp = builder.create<arith::ConstantOp>(
      builder.getUnknownLoc(), builder.getI32Type(), builder.getI32IntegerAttr(1));
  auto castRetOp = builder.create<arith::IndexCastOp>(
      builder.getUnknownLoc(), builder.getIndexType(), constoneOp.getResult());
  auto constRetOp = builder.create<arith::ConstantOp>(
      builder.getUnknownLoc(), builder.getI32Type(), builder.getI32IntegerAttr(42));
  
  auto kernelLaunchOp = builder.create<gpu::LaunchOp>(
      builder.getUnknownLoc(), 
      castRetOp.getResult(), castRetOp.getResult(), castRetOp.getResult(), 
      castRetOp.getResult(), castRetOp.getResult(), castRetOp.getResult());
  
  mlir::Region &body = kernelLaunchOp.getBody();
  mlir::Block *block = &body.front();
  builder.setInsertionPointToStart(block);
  builder.create<gpu::PrintfOp>(
      builder.getUnknownLoc(), 
      builder.getStringAttr("Hi There: "), 
      mlir::ValueRange{});
  builder.create<mlir::gpu::TerminatorOp>(builder.getUnknownLoc());
  builder.setInsertionPointToEnd(entryBlock);
  
  // Return the constant value
  builder.create<func::ReturnOp>(builder.getUnknownLoc(), constRetOp.getResult());
  
  if(failed(verify(module.get()))) {
    llvm::errs() << "Error: Verification Failed\n";
    return 0;
  }
  
  // Print the MLIR code to stdout
  llvm::outs() << "=== Original Module ===\n";
  module->dump();
  
  // First pass: Convert func and arith, and outline GPU kernels
  PassManager pm(&context);
  pm.addPass(createGpuKernelOutliningPass());
  pm.nest<mlir::gpu::GPUModuleOp>().addPass(createConvertGpuOpsToNVVMOps());
  GpuNVVMAttachTargetOptions gputargetOptions;
  gputargetOptions.chip = "sm_90";
  gputargetOptions.triple = "nvptx64-nvidia-cuda";
  pm.addPass(createGpuNVVMAttachTarget(gputargetOptions));
  
  pm.addPass(createArithToLLVMConversionPass());
  pm.addPass(createConvertIndexToLLVMPass());
  pm.addPass(createUBToLLVMConversionPass());

  
  
  
  pm.addPass(createConvertNVVMToLLVMPass());
  pm.addPass(createConvertToLLVMPass());
  pm.addPass(createGpuModuleToBinaryPass());
  pm.addPass(createGpuToLLVMConversionPass());
  
  pm.addPass(createConvertFuncToLLVMPass());
  pm.addPass(createReconcileUnrealizedCastsPass());
  
  if (mlir::failed(pm.run(*module))) {
    llvm::errs() << "Failed to convert GPU to NVVM\n";
    return 1;
  }
  
  llvm::outs() << "\n=== Final Module (with NVVM) ===\n";
  module->dump();
  
  return 0;
}
