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
	    Param(std::string n) : name(n) {}
	};

	using Params = std::vector<Param>;
	using Statements = std::vector<StmtPtr>;

	struct Body {
	    Statements stmts;
	    Body(Statements s) : stmts(mv(s)) {}
	};

	struct FunctionDef {	    
	    std::string name;
	    Params params;
	    StmtPtr body;
	    FunctionDef(std::string name, Params params, StmtPtr body)
	    : name(std::move(name)), params(std::move(params)),
            body(std::move(body)) {}
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
