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

#include "mlir/IR/PatternMatch.h"
#include "mlir/Support/LogicalResult.h"
#include "mlir/Transforms/GreedyPatternRewriteDriver.h"

#include "Dialect.h.inc"

using namespace mlir;
using namespace mydialect;


#include <iostream>

using namespace std;

#define GET_OP_CLASSES
#include "Ops.h.inc"

#include "Dialect.cpp.inc"
#include "llvm/ADT/TypeSwitch.h"

#define GET_OP_CLASSES
#include "Ops.cpp.inc"

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

using namespace mlir;
using namespace mlir::mydialect;


namespace
{
    #include "Rewrites.inc"
}

int main()
{

    MLIRContext context;
    context.getOrLoadDialect<mydialect::MyDialect>();
    context.getOrLoadDialect<func::FuncDialect>();
    context.getOrLoadDialect<arith::ArithDialect>();

    PassManager pm(&context);

    OwningOpRef<ModuleOp> module = ModuleOp::create(UnknownLoc::get(&context));
    OpBuilder builder(&context);

    auto funcType = builder.getFunctionType({}, {});
    auto funcOp = builder.create<func::FuncOp>(builder.getUnknownLoc(), "main", funcType);

    module->push_back(funcOp);
    Block *entryBlock = funcOp.addEntryBlock();
    builder.setInsertionPointToStart(entryBlock);

    mlir::Type intType = builder.getIntegerType(32);
    mlir::Attribute value = builder.getIntegerAttr(intType, 33);

    builder.create<mydialect::Const>(builder.getUnknownLoc(), intType, value);

    builder.create<func::ReturnOp>(builder.getUnknownLoc());

    module->dump();
    verify(module.get());

    RewritePatternSet patterns(&context);
    populateWithGenerated(patterns); // Automatically adds all generated patterns

    if (failed(applyPatternsAndFoldGreedily(module.get(), std::move(patterns))))
    {
        llvm::errs() << "Pattern application failed.\n";
        return 1;
    }

    module->dump();

    return 0;
}