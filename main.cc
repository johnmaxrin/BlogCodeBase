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

namespace mlir
{
    namespace mydialect
    {
#define GEN_PASS_DEF_CONVERTMYDIALECT2ARITH
#include "Passes.h.inc"
    } // namespace mydialect
} // namespace mlir

struct convertmydialect2arith : public mlir::mydialect::impl::convertmydialect2arithBase<convertmydialect2arith>
{
    using convertmydialect2arithBase::convertmydialect2arithBase;
    void runOnOperation() override
    {
        auto mod = getOperation();
        mod->walk([&](mlir::mydialect::Const constOpIter)
                  {
                OpBuilder b(constOpIter);

                auto newOp = b.create<mlir::arith::ConstantOp>(b.getUnknownLoc(), b.getI32Type(),
      b.getI32IntegerAttr(33)); 

            
            constOpIter->replaceAllUsesWith(newOp);
            constOpIter->erase();
            
      });

        
    
    }
};

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
    builder.create<mydialect::Const>(builder.getUnknownLoc());

    builder.create<func::ReturnOp>(builder.getUnknownLoc());

    module->dump();
    verify(module.get());

    pm.addPass(createconvertmydialect2arith());
    if (failed(pm.run(*module)))
    {
        llvm::errs() << "Failed to run passes\n";
        return 1;
    }

    module->dump();

    return 0;
}
