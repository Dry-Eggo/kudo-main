#include "defines.hpp"
#include "nodes.hpp"
#include <cbackend.hpp>
#include <format>
#include <iostream>

using namespace Kudo::CodeGen;
using namespace Kudo::Language;
void CBackend::parse() {
  for (StmtPtr &stmt : program) {
    if (is<uniq(FunctionDef)>(stmt->data)) {
      auto func = as(uniq(FunctionDef), stmt);
      auto code = gen_function(func);
      std::cout << code;
    }
  }
}

std::string CBackend::gen_function(FunctionDef *func) {
    std::string code;
    code += std::format("\nvoid {} () {{\n", func->name);
    auto body_expr = as(uniq(ExprStmt), func->body);
    code += gen_expr(body_expr->expr.get());
    code += std::format("}}\n");
    return code;
}

std::string CBackend::gen_stmt(Stmt* stmt) {
    std::string code;
    if (is<uniq(VariableDecl)>(stmt->data)) {
	auto var_decl = as(uniq(VariableDecl), stmt);
	auto value    = gen_expr(var_decl->expr.get());
	code += std::format("int {} = {};\n", var_decl->name, value);
    }
    return code;
}

std::string CBackend::gen_expr(Expr* expr) {
    std::string code;
    if (is<uniq(Body)>(expr->data)) {
	auto body = as(uniq(Body), expr);
	std::cout << "Here. Body Size: " << body->stmts.size() << "\n";
	for (auto& stmt: body->stmts) {
	    code += gen_stmt(stmt.get());
	}
    } else if (is<uniq(Integer)>(expr->data)) {
	auto integer = as(uniq(Integer), expr);
	code += std::format("{}", integer->value);
    }
    return code;
}
