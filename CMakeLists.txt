cmake_minimum_required(VERSION 3.20)
project(mlir_gpu_example)

find_package(MLIR REQUIRED CONFIG)
find_package(LLVM REQUIRED CONFIG)

message(STATUS "MLIR_DIR: ${MLIR_DIR}")
message(STATUS "LLVM_DIR: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${MLIR_INCLUDE_DIRS})

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_executable(app main.cc)

set_property(TARGET app PROPERTY CXX_STANDARD 17)

# Link against the correct MLIR library names
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
   
    MLIRLLVMToLLVMIRTranslation
    
    MLIRGPUToLLVMIRTranslation
    

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
