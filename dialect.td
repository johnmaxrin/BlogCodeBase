
include "mlir/IR/DialectBase.td"
include "mlir/IR/AttrTypeBase.td"
include "mlir/IR/SymbolInterfaces.td"
include "mlir/IR/OpBase.td"
include "mlir/IR/BuiltinAttributes.td"

include "mlir/Pass/PassBase.td"

def MyDialect : Dialect {
    let summary = "A sample dialect for understanding";
    let name = "mydialect";
    let cppNamespace = "mlir::mydialect";
}


class MyDialectOp<string mnemonic> : Op<MyDialect, mnemonic>
{
  let summary = "Operation Class";
}

def MyDialect_Const : MyDialectOp<"const">
{
  let summary = "Returns a constant"; 
}

def convertmydialect2arith : Pass<"convert-mydialect-to-arith", "mlir::ModuleOp">{
    let summary = "Convert our dialect operations to arith dialect";
    let dependentDialects = ["mlir::mydialect::MyDialect"];
}
