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
    shr(VarMap) parent;
    std::unordered_map<std::string, shr(VariableMeta)> vars;

    VarMap(shr(VarMap) parent) {
	this->parent = parent;
	vars = {};
    }
    
    shr(VariableMeta) get_var(std::string k) {
	if (vars.count(k) == 0) {
	    if (parent) {
		return parent->get_var(k);
	    }	    
	}	
	return vars[k];
    }
    void add_var(std::string k, shr(VariableMeta) v) {
	vars[k] = v;
    }
    bool has_var(std::string k) {
	if (vars.count(k) == 0) {
	    if (parent) {
		return parent->has_var(k);
	    }
	    return false;
	}
	return true;
    }
};

struct FunctionTable {
    std::unordered_map<std::string, shr(FunctionMeta)> funcs;
    shr(FunctionMeta) get_func(std::string k) {
	return funcs[k];
    }
    void add_func(std::string k, shr(FunctionMeta) v) {
	funcs[k] = v;
    }
    bool has_func(std::string k) {
	return funcs.count(k) != 0;
    }
};

namespace Kudo::CodeGen {
    struct CBackend {
	// Output file
	std::string                  output;
	std::string                  out_header;
	std::string                  includes;
	std::string                  body;
	// Core symbols
	std::string                  header_path;
	std::string                  build_directory;
	
	Language::Statements         program;
	
	Types::TypeMap               types;
	shr(VarMap)                  global_context;
	shr(VarMap)                  current_context;
	FunctionTable                functions;
	
	std::vector<SemanticError>   errors;
	std::vector<std::string>     source;
	
	CBackend(Language::Statements p, std::vector<std::string> source) : program(mv(p)), source(mv(source)), output(""), includes(""), body("") {
	    types = TypeMap();
	}

	// context
	void new_ctx();
	void close_ctx();
	
	void parse();
	std::string spread_params(Language::Params, bool);
	CResult gen_function(Language::FunctionDef *func);
	CResult gen_expr(Language::Expr* expr);
	CResult gen_stmt(Language::Stmt* stmt);
	
	std::string type_tostr(Type* type);
	std::string type_torepr(std::string);
	bool type_match(shr(Type) t1, shr(Type) t2);

	// errors
	void underline(std::string line, Span s);
	void format_typemismatch(Semantic::TypeMisMatch err);
	void format_redefinition(Semantic::Redefinition err);
	void format_undeclared(Semantic::Undeclared err);
	void format_undeclaredf(Semantic::UndeclaredFunction err);
	void format_invpc(Semantic::InvalidParameterCount err);
	void add_error(SemanticError e);
	void flush(bool safe = true);
    };
} // namespace Kudo::CodeGen
