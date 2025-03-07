include "mlir/IR/DialectBase.td"
include "mlir/IR/PatternBase.td"
include "mlir/IR/AttrTypeBase.td"
include "mlir/IR/SymbolInterfaces.td"
include "mlir/IR/OpBase.td"
include "mlir/IR/BuiltinAttributes.td"

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
  let arguments = (ins 
                  AnyAttr : $op1
                  );

  let results = (outs 
                  AnyType : $res
  );
}

def MyDialect_Const2 : MyDialectOp<"const2">
{
  let summary = "Returns a constant 2"; 
  let arguments = (ins 
                    AnyAttr : $op
                  );
  let results = (outs 
                  AnyType : $res
  );
}


def const1toconst2: Pat<(MyDialect_Const $a), (MyDialect_Const2 $a)>; // Created the pattern. 





