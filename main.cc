#include "mlir/IR/Builders.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Transforms/DialectConversion.h"
#include "mlir/Conversion/LLVMCommon/TypeConverter.h"   
#include "mlir/Conversion/Passes.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/Dialect/ControlFlow/IR/ControlFlow.h"
#include "mlir/Dialect/GPU/IR/GPUDialect.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/Dialect/LLVMIR/NVVMDialect.h"
#include "mlir/Dialect/GPU/Transforms/Passes.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/Target/LLVM/NVVM/Target.h"

#include "mlir/IR/Verifier.h"
// Conversions
#include "mlir/Conversion/ArithToLLVM/ArithToLLVM.h"
#include "mlir/Conversion/GPUToNVVM/GPUToNVVMPass.h"
#include "mlir/Conversion/GPUCommon/GPUCommonPass.h"
#include "mlir/Conversion/IndexToLLVM/IndexToLLVM.h"
#include "mlir/Conversion/FuncToLLVM/ConvertFuncToLLVMPass.h"
#include "mlir/Conversion/FuncToLLVM/ConvertFuncToLLVM.h"
#include "mlir/Conversion/UBToLLVM/UBToLLVM.h"
#include "mlir/Conversion/ControlFlowToLLVM/ControlFlowToLLVM.h"
#include "mlir/Conversion/MemRefToLLVM/MemRefToLLVM.h"
#include "mlir/Conversion/ReconcileUnrealizedCasts/ReconcileUnrealizedCasts.h"
#include "mlir/Conversion/SCFToGPU/SCFToGPUPass.h"
#include "mlir/Conversion/SCFToControlFlow/SCFToControlFlow.h"
#include "mlir/Conversion/UBToLLVM/UBToLLVM.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Pass/Pass.h"

#include "mlir/Target/LLVMIR/Dialect/Builtin/BuiltinToLLVMIRTranslation.h"
#include "mlir/Target/LLVMIR/Dialect/NVVM/NVVMToLLVMIRTranslation.h"
#include "mlir/Target/LLVMIR/Dialect/LLVMIR/LLVMToLLVMIRTranslation.h"
#include "mlir/Target/LLVMIR/Dialect/GPU/GPUToLLVMIRTranslation.h"

// Extension registration headers
#include "mlir/Dialect/Func/Extensions/AllExtensions.h"

using namespace mlir;

int main() {
  MLIRContext context(MLIRContext::Threading::DISABLED);
  
  // Register dialects
  context.loadDialect<arith::ArithDialect>();
  context.loadDialect<func::FuncDialect>();
  context.loadDialect<gpu::GPUDialect>();
  context.loadDialect<LLVM::LLVMDialect>();
  context.loadDialect<NVVM::NVVMDialect>();
  context.loadDialect<memref::MemRefDialect>();
  context.loadDialect<cf::ControlFlowDialect>();
	context.loadDialect<scf::SCFDialect>();
  
  // Register interfaces and translations
  DialectRegistry registry;
  
  // Register conversion interfaces
  registerConvertNVVMToLLVMInterface(registry);
  registerConvertFuncToLLVMInterface(registry);
	ub::registerConvertUBToLLVMInterface(registry);
  cf::registerConvertControlFlowToLLVMInterface(registry);
  arith::registerConvertArithToLLVMInterface(registry);
  registerConvertMemRefToLLVMInterface(registry);
  
  // Register target interfaces
  NVVM::registerNVVMTargetInterfaceExternalModels(registry);
  
  // Register translations - CRITICAL for GPU module conversion
  mlir::registerBuiltinDialectTranslation(registry);
  mlir::registerNVVMDialectTranslation(registry);
  mlir::registerLLVMDialectTranslation(registry);
  mlir::registerGPUDialectTranslation(registry);
  
  func::registerAllExtensions(registry);
  context.appendDialectRegistry(registry);
  
  // Create a module
  OwningOpRef<ModuleOp> module = ModuleOp::create(UnknownLoc::get(&context));
  OpBuilder builder(&context);
  
  // Define the function type (takes no arguments, returns i32)
  auto funcType = builder.getFunctionType({}, builder.getI32Type());
  
  // Create a function named "main"
  auto funcOp = builder.create<func::FuncOp>(
      builder.getUnknownLoc(), "main", funcType);
  
  module->push_back(funcOp);
  
  // Create a new block inside the function
  Block *entryBlock = funcOp.addEntryBlock();
  builder.setInsertionPointToStart(entryBlock);

  // Create constants
  auto constoneOp = builder.create<arith::ConstantOp>(
      builder.getUnknownLoc(), builder.getI32Type(), builder.getI32IntegerAttr(1));
  auto castRetOp = builder.create<arith::IndexCastOp>(
      builder.getUnknownLoc(), builder.getIndexType(), constoneOp.getResult());
  auto constRetOp = builder.create<arith::ConstantOp>(
      builder.getUnknownLoc(), builder.getI32Type(), builder.getI32IntegerAttr(42));
	

	  auto constupOp = builder.create<arith::ConstantOp>(
      builder.getUnknownLoc(), builder.getI32Type(), builder.getI32IntegerAttr(100));
  auto castupOp = builder.create<arith::IndexCastOp>(
      builder.getUnknownLoc(), builder.getIndexType(), constupOp.getResult());

			  auto constlwOp = builder.create<arith::ConstantOp>(
      builder.getUnknownLoc(), builder.getI32Type(), builder.getI32IntegerAttr(0));
  auto castlwOp = builder.create<arith::IndexCastOp>(
      builder.getUnknownLoc(), builder.getIndexType(), constlwOp.getResult());

	
			// create a vector addition loop. 
			llvm::outs() << "Hi\n";
	//std::optional<ArrayAttr> devicemap = std::nullopt;
	auto gpuThreadX = gpu::GPUThreadMappingAttr::get(&context, gpu::MappingId::DimX);
  ArrayAttr mappingAttr = builder.getArrayAttr({gpuThreadX});
	auto scffor = builder.create<scf::ParallelOp>(builder.getUnknownLoc(),
																							ValueRange{castlwOp},
																							ValueRange{castupOp},
																							ValueRange{castRetOp},
																							ValueRange{},
																						  [&](OpBuilder &b, Location innerLoc, ValueRange ivs, ValueRange idntknw ){
																								b.create<gpu::PrintfOp>(builder.getUnknownLoc(), builder.getStringAttr("Hi"), mlir::ValueRange{});
																								b.create<scf::ReduceOp>(innerLoc);
																							}
																						);

			llvm::outs() << "Hi 2\n";
  // // Create GPU launch
  // auto kernelLaunchOp = builder.create<gpu::LaunchOp>(
  //     builder.getUnknownLoc(), 
  //     castRetOp.getResult(), castRetOp.getResult(), castRetOp.getResult(), 
  //     castRetOp.getResult(), castRetOp.getResult(), castRetOp.getResult());
  
  // mlir::Region &body = kernelLaunchOp.getBody();
  // mlir::Block *block = &body.front();
  // builder.setInsertionPointToStart(block);
  // builder.create<gpu::PrintfOp>(
  //     builder.getUnknownLoc(), 
  //     builder.getStringAttr("Hi There: "), 
  //     mlir::ValueRange{});
  // builder.create<mlir::gpu::TerminatorOp>(builder.getUnknownLoc());
  // builder.setInsertionPointToEnd(entryBlock);
  
  // Return the constant value
  builder.create<func::ReturnOp>(builder.getUnknownLoc(), constRetOp.getResult());
  
  if(failed(verify(module.get()))) {
    llvm::errs() << "Error: Verification Failed\n";
    return 1;
  }
  
  llvm::outs() << "=== Original Module ===\n";
  module->dump();
  
  // Configure pass pipeline
  PassManager pm(&context);
  
	pm.nest<func::FuncOp>().addPass(createGpuMapParallelLoopsPass()); // map parallel loops	
	pm.addPass(mlir::createConvertParallelLoopToGpuPass());
	
  // Step 1: Outline GPU kernels into separate gpu.module
  pm.addPass(createGpuKernelOutliningPass());
  
  // Step 2: Lower GPU ops to NVVM within gpu.module
  pm.nest<mlir::gpu::GPUModuleOp>().addPass(createConvertGpuOpsToNVVMOps());
  
  // Step 3: Attach NVVM target with correct libdevice path
  GpuNVVMAttachTargetOptions gputargetOptions;
  gputargetOptions.chip = "sm_61";
  gputargetOptions.triple = "nvptx64-nvidia-cuda";
  // Fix the libdevice path - adjust this to your CUDA installation
  // Common locations:
  // - /usr/local/cuda/nvvm/libdevice/libdevice.10.bc
  // - /opt/cuda/nvvm/libdevice/libdevice.10.bc
  // Or set to empty string to skip libdevice linking
  pm.addPass(createGpuNVVMAttachTarget(gputargetOptions));

	// pm.addPass(mlir::createConvertSCFToCFPass());
  
  // Step 4: Convert NVVM to LLVM
  pm.addPass(createConvertNVVMToLLVMPass());
  
  // Step 5: Serialize GPU module to binary (PTX/CUBIN)
  pm.addPass(createGpuModuleToBinaryPass());
  pm.addPass(createGpuToLLVMConversionPass());
  
  // Step 6: Lower host-side operations
  pm.addPass(createArithToLLVMConversionPass());
  pm.addPass(createConvertIndexToLLVMPass());
  pm.addPass(createUBToLLVMConversionPass());

	pm.addPass(mlir::createSCFToControlFlowPass());
  
  // Step 7: Convert func and GPU launch ops to LLVM
  pm.addPass(createConvertFuncToLLVMPass());
  
  // Step 8: Clean up unrealized casts
  pm.addPass(createReconcileUnrealizedCastsPass());
  
  if (mlir::failed(pm.run(*module))) {
    llvm::errs() << "Failed to convert GPU to NVVM\n";
    return 1;
  }
  
  llvm::outs() << "\n=== Final Module (with NVVM) ===\n";
  module->dump();
  
  return 0;
}
