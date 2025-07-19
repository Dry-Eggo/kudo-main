#pragma once
#include <defines.hpp>
#include <unordered_map>

namespace Kudo::CodeGen {
    using Kudo::Language::Params;
    enum SymbolKind {
	Function,
	Variable,
	Alias,
    };

    struct VariableMeta {
	std::string name;
	shr(Type)   type;
	std::string mangled_name;
	Span declaration; // Declaration Location
	SymbolKind  symtype;
	
	VariableMeta(std::string n, shr(Type) t, Span d, SymbolKind k)
	: name(mv(n)), type(t), mangled_name(name), declaration(d), symtype(k) {}
    };

    struct FunctionMeta {
	std::string name;
	std::string mangled_name;

	shr(Type)   return_type;
	Span        declaration;
	Params      params;
	bool        variadic;
	
	FunctionMeta(std::string n, shr(Type) t, Span s, Params p, bool v = false)
	: name(mv(n)), mangled_name(mv(n)), return_type(t), declaration(s), params(p), variadic(v) {}
    };

    struct VarMap {
	shr(VarMap) parent;
	std::unordered_map<std::string, shr(VariableMeta)> vars;

	VarMap(shr(VarMap) parent);	
	shr(VariableMeta) get_var(std::string k);
	void add_var(std::string k, shr(VariableMeta) v);
	bool has_var(std::string k);
    };

    struct FunctionTable {
	std::unordered_map<std::string, shr(FunctionMeta)> funcs;
	shr(FunctionMeta) get_func(std::string k);
	void add_func(std::string k, shr(FunctionMeta) v);
	bool has_func(std::string k);
    };
}
