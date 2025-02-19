
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


class MyDialectOp<string mnemonic, list<Trait> traits = []> : Op<MyDialect, mnemonic, traits>
{
  let summary = "Operation Class";
}

def MyDialect_FuncOp : MyDialectOp<"func",[IsolatedFromAbove, SymbolTable, Symbol]>
{
    let summary = "Defines a function in the Starplat Dialect";
    let description = [{This operation represents a function definiton in the Starplat dialect.
                        It contains a name, other related arguments and an optional return type.
                        It has a single region for the function body.
                        }];

  let arguments = (ins 
                    SymbolNameAttr:$sym_name,
                    TypeAttrOf<FunctionType>:$funcType
                  );

  let regions = (region AnyRegion:$body);

}

def MyDialect_Const : MyDialectOp<"const", [Symbol]>
{
  let summary = "Returns a constant"; 
  let arguments = (ins SymbolNameAttr:$sym_name);
  let results = (outs AnyType);
}

