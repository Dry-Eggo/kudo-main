#pragma once

#include <defines.hpp>
#include <nodes.hpp>
#include <types.hpp>

namespace Kudo::CodeGen {
    struct CBackend {
	Language::Statements program;
	CBackend(Language::Statements p) : program(mv(p)) {}
	void parse();
	std::string gen_function(Language::FunctionDef *func);
	std::string gen_expr(Language::Expr* expr);
	std::string gen_stmt(Language::Stmt* stmt);
	std::string type_tostr(Type type);
    };
} // namespace Kudo::CodeGen
