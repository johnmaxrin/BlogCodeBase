// Create Dialect
// Lower ops of that dialect to arith
// From that to LLVM

#include "mlir/IR/Builders.h"
#include "mlir/IR/Dialect.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/BuiltinOps.h"

#include "mlir/IR/OpImplementation.h"
#include "mlir/IR/DialectImplementation.h"

#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Arith/IR/Arith.h"

#include "mlir/Conversion/Passes.h"
#include "mlir/Pass/PassManager.h"

#include "mlir/IR/Verifier.h"

#include "Dialect.h.inc"

#include <iostream>

using namespace std;

namespace mlir
{
    namespace midialect
    {
#define GEN_PASS_DECL_CONVERTMYDIALECT2ARITH
#include "Passes.h.inc"

#define GEN_PASS_REGISTRATION
#include "Passes.h.inc"
    }
}

#define GET_OP_CLASSES
#include "Ops.h.inc"

#include "Dialect.cpp.inc"
#include "llvm/ADT/TypeSwitch.h"

#define GET_OP_CLASSES
#include "Ops.cpp.inc"

using namespace mlir;
using namespace mlir::mydialect;

namespace mlir
{
    namespace mydialect
    {

        void MyDialect::initialize()
        {

            addOperations<
#define GET_OP_LIST
#include "Ops.cpp.inc"
                >();
        }
    }

}


int main()
{

    MLIRContext context;
    context.getOrLoadDialect<mydialect::MyDialect>();
    context.getOrLoadDialect<func::FuncDialect>();
    context.getOrLoadDialect<arith::ArithDialect>();


    OwningOpRef<ModuleOp> module = ModuleOp::create(UnknownLoc::get(&context));
    OpBuilder builder(&context);


    auto funcType = builder.getFunctionType({}, {});
    auto funcOp = builder.create<mydialect::FuncOp>(builder.getUnknownLoc(), "main", funcType);
    module->push_back(funcOp);
    
    auto &entryBlock = funcOp.getBody().emplaceBlock();
    builder.setInsertionPointToStart(&entryBlock);

    mlir::SymbolTable symbolTable(funcOp);

    auto constOp = builder.create<mydialect::Const>(builder.getUnknownLoc(), builder.getI32Type(),builder.getStringAttr("my_var"));

    symbolTable.insert(constOp);


    builder.create<func::ReturnOp>(builder.getUnknownLoc());

    if(auto found = symbolTable.lookup("my_var")){
        llvm::outs() << "Found: " << found->getName() <<"\n";
    }


    return 0;
}
