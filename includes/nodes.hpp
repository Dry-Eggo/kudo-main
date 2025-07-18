#pragma once

#include <defines.hpp>
#include <memory>
#include <string>
#include <variant>
#include <vector>

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

struct VariableDecl {
  bool is_const = true;
  std::string name;
  ExprPtr expr;
  VariableDecl(std::string n, ExprPtr e)
      : name(std::move(n)), expr(std::move(e)) {}
};

struct Integer {
  int value;
  Integer(int v) : value(v) {}
};

struct StringN {
  std::string value;
  StringN(std::string v) : value(mv(v)) {}
};

struct ExprStmt {
  ExprPtr expr;
  ExprStmt(ExprPtr e) : expr(std::move(e)) {}
};

struct Expr {
  ExprVariant data;
  Expr(ExprVariant d) : data(std::move(d)) {}
};

struct Stmt {
  StmtVariant data;
  Stmt(StmtVariant d) : data(std::move(d)) {}
};
} // namespace Language
} // namespace Kudo
