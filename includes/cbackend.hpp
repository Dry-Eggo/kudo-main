#pragma once

#include <defines.hpp>
#include <nodes.hpp>
#include <types.hpp>
#include <errors.hpp>
#include <format>
#include <format>
using Kudo::Semantic::SemanticError;
struct CResult;
struct FunctionMeta;
struct VariableMeta;

struct VarMap {
    std::unordered_map<std::string, shr(VariableMeta)> vars;
    shr(VariableMeta) get_var(std::string k) {
	return vars[k];
    }
    void add_var(std::string k, shr(VariableMeta) v) {
	vars[k] = v;
    }
    bool has_var(std::string k) {
	return vars.count(k) != 0;
    }
};

namespace Kudo::CodeGen {
    struct CBackend {
	std::string  output;
	Language::Statements program;
	Types::TypeMap       types;
	std::vector<SemanticError>   errors;
	std::vector<std::string> source;
	VarMap global_context;
	
	CBackend(Language::Statements p, std::vector<std::string> source) : program(mv(p)), source(mv(source)) {
	    types = TypeMap();
	}
	
	void parse();
	
	CResult gen_function(Language::FunctionDef *func);
	CResult gen_expr(Language::Expr* expr);
	CResult gen_stmt(Language::Stmt* stmt);
	
	std::string type_tostr(Type* type);
	bool type_match(shr(Type) t1, shr(Type) t2);

	// errors
	void format_typemismatch(Semantic::TypeMisMatch err);
	void format_redefinition(Semantic::Redefinition err);
	void add_error(SemanticError e);
	void flush();
    };
} // namespace Kudo::CodeGen
