#include "bugle/Ref.h"
#include "bugle/Type.h"
#include "bugle/Var.h"
#include "llvm/ADT/APInt.h"
#include <vector>

#ifndef BUGLE_EXPR_H
#define BUGLE_EXPR_H

namespace llvm {
 
class Value;

}

namespace bugle {

class Function;
class GlobalArray;
class Var;

class Expr {
public:
  enum Kind {
    BVConst,
    BoolConst,
    GlobalArrayRef,
    Pointer,
    Load,
    VarRef,
    Call,
    BVExtract,
    BVZExt,
    BVSExt,

    // Unary
    ArrayId,
    ArrayOffset,
    BVToFloat,
    FloatToBV,
    BVToPtr,
    PtrToBV,
    BVToBool,
    BoolToBV,

    UnaryFirst = ArrayId,
    UnaryLast = BoolToBV,

    // Binary
    BVAdd,
    BVSub,
    BVMul,
    BVSDiv,
    BVUDiv,
    BVConcat,
    BVSgt,

    BinaryFirst = BVAdd,
    BinaryLast = BVSgt
  };

  unsigned refCount;

private:
  Type type;

protected:
  Expr(Type type) : type(type) {}

public:
  virtual ~Expr() {}
  virtual Kind getKind() const = 0;
  const Type &getType() const { return type; }

  static bool classof(const Expr *) { return true; }
};

#define EXPR_KIND(kind) \
  Kind getKind() const { return kind; } \
  static bool classof(const Expr *E) { return E->getKind() == kind; } \
  static bool classof(const kind##Expr *) { return true; }

class BVConstExpr : public Expr {
  BVConstExpr(const llvm::APInt &bv) :
    Expr(Type(Type::BV, bv.getBitWidth())), bv(bv) {}
  llvm::APInt bv;

public:
  static ref<Expr> create(const llvm::APInt &bv);
  static ref<Expr> create(unsigned width, uint64_t val, bool isSigned = false);
  static ref<Expr> createZero(unsigned width);

  EXPR_KIND(BVConst)
  const llvm::APInt &getValue() const { return bv; }
};

class BoolConstExpr : public Expr {
  BoolConstExpr(bool val) : Expr(Type(Type::Bool)), val(val) {}
  bool val;

public:
  static ref<Expr> create(bool val);

  EXPR_KIND(BoolConst)
  bool getValue() const { return val; }
};

class GlobalArrayRefExpr : public Expr {
  GlobalArrayRefExpr(GlobalArray *array) :
    Expr(Type(Type::ArrayId)), array(array) {}
  GlobalArray *array;

public:
  static ref<Expr> create(GlobalArray *array);

  EXPR_KIND(GlobalArrayRef)
  GlobalArray *getArray() const { return array; }
};

class PointerExpr : public Expr {
  PointerExpr(ref<Expr> array, ref<Expr> offset) :
    Expr(Type(Type::Pointer, offset->getType().width)),
    array(array), offset(offset) {}
  ref<Expr> array, offset;

public:
  static ref<Expr> create(ref<Expr> array, ref<Expr> offset);

  EXPR_KIND(Pointer)
  ref<Expr> getArray() const { return array; }
  ref<Expr> getOffset() const { return offset; }
};

class LoadExpr : public Expr {
  LoadExpr(ref<Expr> array, ref<Expr> offset) :
    Expr(Type(Type::BV, 8)),
    array(array), offset(offset) {}
  ref<Expr> array, offset;

public:
  static ref<Expr> create(ref<Expr> array, ref<Expr> offset);

  EXPR_KIND(Load)
  ref<Expr> getArray() const { return array; }
  ref<Expr> getOffset() const { return offset; }
};

/// Local variable reference.  Used for phi nodes, parameters and return
/// variables.
class VarRefExpr : public Expr {
  Var *var;
  VarRefExpr(Var *var) : Expr(var->getType()), var(var) {}

public:
  static ref<Expr> create(Var *var);
  EXPR_KIND(VarRef)
  Var *getVar() const { return var; }
};

class BVExtractExpr : public Expr {
  BVExtractExpr(ref<Expr> expr, unsigned offset, unsigned width) :
    Expr(Type(Type::BV, width)), expr(expr), offset(offset) {}
  ref<Expr> expr;
  unsigned offset;

public:
  static ref<Expr> create(ref<Expr> expr, unsigned offset, unsigned width);

  EXPR_KIND(BVExtract)
  ref<Expr> getSubExpr() const { return expr; }
  unsigned getOffset() const { return offset; }
};

class BVZExtExpr : public Expr {
  BVZExtExpr(unsigned width, ref<Expr> expr) :
    Expr(Type(Type::BV, width)), expr(expr) {}
  ref<Expr> expr;

public:
  static ref<Expr> create(unsigned width, ref<Expr> expr);
  EXPR_KIND(BVZExt)
  ref<Expr> getSubExpr() const { return expr; }
};

class BVSExtExpr : public Expr {
  BVSExtExpr(unsigned width, ref<Expr> expr) :
    Expr(Type(Type::BV, width)), expr(expr) {}
  ref<Expr> expr;

public:
  static ref<Expr> create(unsigned width, ref<Expr> expr);
  EXPR_KIND(BVSExt)
  ref<Expr> getSubExpr() const { return expr; }
};

class UnaryExpr : public Expr {
  ref<Expr> expr;

protected:
  UnaryExpr(Type type, ref<Expr> expr) :
    Expr(type), expr(expr) {}

public:
  ref<Expr> getSubExpr() const { return expr; }
  static bool classof(const Expr *E) {
    Kind k = E->getKind();
    return k >= UnaryFirst && k <= UnaryLast;
  }
  static bool classof(const UnaryExpr *) { return true; }
};

#define UNARY_EXPR(kind) \
  class kind##Expr : public UnaryExpr { \
    kind##Expr(Type type, ref<Expr> expr) : UnaryExpr(type, expr) {} \
\
  public: \
    static ref<Expr> create(ref<Expr> var); \
    EXPR_KIND(kind) \
  };

UNARY_EXPR(ArrayId)
UNARY_EXPR(ArrayOffset)
UNARY_EXPR(BVToFloat)
UNARY_EXPR(FloatToBV)
UNARY_EXPR(BVToPtr)
UNARY_EXPR(PtrToBV)
UNARY_EXPR(BVToBool)
UNARY_EXPR(BoolToBV)

#undef UNARY_EXPR

class BinaryExpr : public Expr {
  ref<Expr> lhs, rhs;

protected:
  BinaryExpr(Type type, ref<Expr> lhs, ref<Expr> rhs) :
    Expr(type), lhs(lhs), rhs(rhs) {}

public:
  ref<Expr> getLHS() const { return lhs; }
  ref<Expr> getRHS() const { return rhs; }
  static bool classof(const Expr *E) {
    Kind k = E->getKind();
    return k >= BinaryFirst && k <= BinaryLast;
  }
  static bool classof(const BinaryExpr *) { return true; }
};

#define BINARY_EXPR(kind) \
  class kind##Expr : public BinaryExpr { \
    kind##Expr(Type type, ref<Expr> lhs, ref<Expr> rhs) : \
      BinaryExpr(type, lhs, rhs) {} \
\
  public: \
    static ref<Expr> create(ref<Expr> lhs, ref<Expr> rhs); \
    EXPR_KIND(kind) \
  };

BINARY_EXPR(BVAdd)
BINARY_EXPR(BVSub)
BINARY_EXPR(BVMul)
BINARY_EXPR(BVSDiv)
BINARY_EXPR(BVUDiv)
BINARY_EXPR(BVConcat)
BINARY_EXPR(BVSgt)

#undef BINARY_EXPR

class CallExpr : public Expr {
  Function *callee;
  std::vector<ref<Expr> > args;

public:
  EXPR_KIND(Call)
};

}

#undef EXPR_KIND

#endif
