cmake_minimum_required(VERSION 3.20)
project(mlir_gpu_example LANGUAGES CXX)

find_package(MLIR REQUIRED CONFIG)
find_package(LLVM REQUIRED CONFIG)

message(STATUS "MLIR_DIR: ${MLIR_DIR}")
message(STATUS "LLVM_DIR: ${LLVM_DIR}")

add_executable(app main.cc)
set_property(TARGET app PROPERTY CXX_STANDARD 17)

target_include_directories(app PRIVATE
  ${MLIR_INCLUDE_DIRS}
  ${LLVM_INCLUDE_DIRS}
)

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
target_compile_definitions(app PRIVATE ${LLVM_DEFINITIONS_LIST})

target_link_libraries(app PRIVATE
    # Core MLIR
    MLIRSupport
    MLIRIR
    MLIRParser
    MLIRPass
    MLIRTransforms
    MLIRAnalysis
    
    # Dialects - use the correct names with "Dialect" suffix
    MLIRArithDialect
    MLIRFuncDialect
    MLIRGPUDialect
    MLIRLLVMDialect
    MLIRNVVMDialect
    MLIRSCFToGPU
    MLIRSCFToControlFlow
   
    MLIRLLVMToLLVMIRTranslation
    
    MLIRGPUToLLVMIRTranslation
    MLIRBuiltinToLLVMIRTranslation 

    # Conversions
    MLIRArithToLLVM
    MLIRFuncToLLVM
    MLIRGPUToNVVMTransforms
    MLIRGPUTransforms
    MLIRReconcileUnrealizedCasts
    MLIRNVVMToLLVM
    MLIRIndexToLLVM 
    MLIRUBToLLVM
    # Extensions - ONLY func extensions
    MLIRFuncAllExtensions
    
    # LLVM libraries
    LLVMSupport
    LLVMCore
)
