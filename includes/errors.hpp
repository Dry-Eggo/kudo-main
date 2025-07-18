#pragma once

#include <defines.hpp>
#include <token.hpp>
#include <variant>
#include <vector>

namespace Kudo::Semantic {
    using Kudo::Language::Span;
    
    struct UnknownType {
	Span span;
	UnknownType(Span s): span(s) {}
    };

    struct TypeMisMatch {
	Span span;
	std::string expected;
	std::string got;
	TypeMisMatch(Span s, std::string e, std::string g): span(s), expected(mv(e)), got(mv(g)) {}
    };

    struct Redefinition {
	Span first;
	Span second;
	std::string symbol_name;
	Redefinition(Span f, Span s, std::string sn): first(f), second(s), symbol_name(sn) {}
    };

    struct Undeclared {
	Span span;
	std::string symbol_name;
	Undeclared(Span s, std::string sn): span(s), symbol_name(sn) {}
    };

    struct UndeclaredFunction {
	Span span;
	std::string symbol_name;
	UndeclaredFunction(Span s, std::string sn): span(s), symbol_name(sn) {}
    };
    
    struct InvalidParameterCount {
	Span callee;
	Span definition;
	int  expected;
	int  got;
	InvalidParameterCount(Span c, Span d, int e, int g): callee(c), definition(d), expected(e), got(g) {}
    };
    
    typedef std::variant<UndeclaredFunction, UnknownType, TypeMisMatch, Redefinition, Undeclared, InvalidParameterCount> SemanticError;
}
