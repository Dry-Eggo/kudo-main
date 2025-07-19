#pragma once

#include <defines.hpp>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <types.hpp>
#include <token.hpp>

using namespace Kudo::CodeGen::Types;
using Kudo::Language::Span;

namespace Kudo {
    namespace Language {
	struct Expr;
	struct Stmt;

	struct Param {
	    std::string name;
	    shr(Type)   type;
	    Span        span;
	    
	    Param(std::string n, shr(Type) t, Span s) : name(n), type(mv(t)), span(s) {}
	};

	using Params = std::vector<Param>;
	using Statements = std::vector<StmtPtr>;

	struct Body {
	    Statements stmts;
	    Body(Statements s) : stmts(mv(s)) {}
	};

	struct ExternDef {
	    enum ExternType { Function, Var, Struct } kind;
	    StmtPtr stmt;
	    ExternDef(ExternType t, StmtPtr s): stmt(mv(s)), kind(t) {}
	};
	
	struct FunctionDef {	    
	    std::string name;
	    Params params;
	    StmtPtr body;
	    shr(Type) return_type;
	    Span    span; /* span covering the name of this function */
	    bool    is_definition;
	    bool    is_variadic;
	    
	    FunctionDef(std::string name, Params params, StmtPtr body, shr(Type) rt, Span s, bool i)
	    : name(std::move(name)), params(std::move(params)), is_variadic(i),
            body(std::move(body)), return_type(mv(rt)), span(s), is_definition(true) {}

	    FunctionDef(std::string name, Params params, shr(Type) rt, Span s, bool v)
	    : name(std::move(name)), params(std::move(params)), body(nullptr), is_variadic(v),
            return_type(mv(rt)), span(s), is_definition(false) {}
	};

	struct FunctionCall {
	    ExprPtr callee;
	    std::vector<ExprPtr> args;

	    FunctionCall(ExprPtr c, std::vector<ExprPtr> a)
	    : callee(mv(c)), args(mv(a)) {}
	};
	
	struct VariableDecl {
	    bool is_const = true;
	    std::string name;
	    ExprPtr expr;
	    shr(Type)    type;
	    
	    VariableDecl(std::string n, ExprPtr e)
	    : name(std::move(n)), expr(std::move(e)) {}
	    
	    VariableDecl(std::string n, ExprPtr e, shr(Type) t)
	    : name(std::move(n)), expr(std::move(e)), type(mv(t)) {}
	};

	struct IdentN {
	    std::string value;
	    IdentN(std::string v) : value(mv(v)) {}
	};
	
	struct Integer {
	    int value;
	    Integer(int v) : value(v) {}
	};

	struct StringN {
	    std::string value;
	    StringN(std::string v) : value(mv(v)) {}
	};

	struct CStringN {
	    std::string value;
	    CStringN(std::string v) : value(mv(v)) {}
	};

	struct BinaryOp {
	    enum BinOp { Add, Sub, Mul, Div, Eq } op;
	    ExprPtr    lhs;
	    ExprPtr    rhs;

	    BinaryOp(BinOp op, ExprPtr l, ExprPtr r): op(op), lhs(mv(l)), rhs(mv(r)) {}
	};

	struct UnaryOp {
	    enum UnoOp { Add, Sub } op;
	    ExprPtr    expr;

	    UnaryOp(UnoOp op, ExprPtr e): op(op), expr(mv(e)) {}
	};
	
	struct ExprStmt {
	    ExprPtr expr;
	    ExprStmt(ExprPtr e) : expr(std::move(e)) {}
	};

	struct Expr {
	    ExprVariant data;
	    Span span;
	    Expr(ExprVariant d, Span s) : data(std::move(d)), span(s) {}
	};

	struct Stmt {
	    StmtVariant data;
	    Span span;
	    Stmt(StmtVariant d, Span s) : data(std::move(d)), span(s) {}
	};
    } // namespace Language
} // namespace Kudo
