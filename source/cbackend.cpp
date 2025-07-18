#include "defines.hpp"
#include "nodes.hpp"
#include <cbackend.hpp>
#include <iostream>

using namespace Kudo::CodeGen;
using namespace Kudo::Language;
using namespace Kudo::Semantic;

struct CResult {
    shr(Type)  type;
    std::string code;
    std::string preamble;
    CResult(std::string c, std::string p, shr(Type) t)
    : type(t), code(mv(c)), preamble(mv(p)) {}
};


struct VariableMeta {
    std::string name;
    shr(Type)   type;
    std::string mangled_name;
    Span declaration; // Declaration Location
    
    VariableMeta(std::string n, shr(Type) t, Span d)
    : name(mv(n)), type(t), mangled_name(name), declaration(d) {}
};

void CBackend::parse() {
    output = "#include <stdio.h>\n";
    for (StmtPtr &stmt : program) {
	if (is<uniq(FunctionDef)>(stmt->data)) {
	    auto func = as(uniq(FunctionDef), stmt);
	    auto code = gen_function(func);
	    output += code.code;
	}
    }
    if (!errors.empty()) {
	flush();
	exit(1);
    }
}

CResult CBackend::gen_function(FunctionDef *func) {
    std::string code;
    code += std::format("\nint {} () {{\n", func->name);
    auto body_expr = as(uniq(ExprStmt), func->body);
    code += gen_expr(body_expr->expr.get()).code;
    code += std::format("}}\n");
    
    return CResult(code, "", NULL);
}

CResult CBackend::gen_stmt(Stmt* stmt) {
    std::string code;
    if (is<uniq(VariableDecl)>(stmt->data)) {
	auto var_decl = as(uniq(VariableDecl), stmt);
	auto value    = gen_expr(var_decl->expr.get());

	if (global_context.has_var(var_decl->name)) {
	    auto var_info = global_context.get_var(var_decl->name);
	    add_error(Redefinition(var_info->declaration, stmt->span, var_decl->name));
	    return CResult("", "", var_info->type);
	}
	
	if (var_decl->type) {
	    auto type_repr = type_tostr(var_decl->type.get());	   
	    auto type_info = types.get_type(type_repr);
	    if (!type_match(type_info, value.type)) {
		auto got = type_tostr(value.type.get());
		add_error(TypeMisMatch(stmt->span, type_repr, got));
	    }
	}
	
	code += std::format("int {} = {}", var_decl->name, value.code);
	global_context.add_var(var_decl->name, mk_s(VariableMeta, var_decl->name, var_decl->type, stmt->span));
    } if (is<uniq(ExprStmt)>(stmt->data)) {
	auto expr_stmt = as(uniq(ExprStmt), stmt);
	auto res       = gen_expr(expr_stmt->expr.get());
	code += res.code;
    }
    return CResult(code, "", NULL);
}

CResult CBackend::gen_expr(Expr* expr) {
    std::string code;
    shr(Type)  type = NULL;
    
    if (is<uniq(Body)>(expr->data)) {
	auto body = as(uniq(Body), expr);
	for (auto& stmt: body->stmts) {
	    code += std::format("{};\n", gen_stmt(stmt.get()).code);
	}
    }
    else if (is<uniq(Integer)>(expr->data)) {
	
	auto integer = as(uniq(Integer), expr);
	code += std::format("{}", integer->value);
	type = types.get_type("int");
	
    }
    else if (is<uniq(CStringN)>(expr->data)) {
	
	auto cstring = as(uniq(CStringN), expr);
	code += std::format("\"{}\"", cstring->value);
	type = types.get_type("cstr");
	
    }
    else if (is<uniq(IdentN)>(expr->data)) {
	// Todo: Check for Variable
	auto ident = as(uniq(IdentN), expr);
	code = ident->value;
    }
    else if (is<uniq(FunctionCall)>(expr->data)) {

	auto fc         = as(uniq(FunctionCall), expr);
	auto ident_expr = gen_expr(fc->callee.get()).code;
	std::string args;
	int n = 0;
	for (auto& arg: fc->args) {
	    auto arg_code = gen_expr(arg.get()).code;
	    args += arg_code;
	    if (n == fc->args.size() - 1) break;
	    args += ",";
	    n++;
	}
	code = std::format("{}({})", ident_expr, args);
    }
    return CResult(code, "", type);
}

std::string CBackend::type_tostr(Type* type) {
    switch(type->type)
    {
	case Types::BaseType::Int: return "int";
	case Types::BaseType::CString: return "cstr";
	case Types::BaseType::String:  return "kudo_string";
	default: return "error_type";
    }
}

bool CBackend::type_match(shr(Type) t1, shr(Type) t2) {
    return (t1->type == t2->type);
}

void CBackend::add_error(SemanticError e) {
    errors.push_back(e);
}

#define to(t, v) std::get<t>(v)

int first_char(std::string s) {
    int n = 0;
    for(; n < s.size(); n++) {
	if (isalnum(s.at(n))) break;
    }
    return n;
}

void CBackend::format_typemismatch(TypeMisMatch err) {
    auto line = source.at(err.span.line-1);
    printf("Error: %s:%d:%d-%d: Cannot Implicitly Convert from '%s' to '%s'. Type Mismatch.\n",
    err.span.filename.c_str(), err.span.line, err.span.column, err.span.offset, err.got.c_str(), err.expected.c_str());
    printf(" %1c |\n", ' ');
    printf(" %1d |%s\n", err.span.line, line.c_str());
    printf(" %1c |", ' ');
    
    underline(line, err.span);
}

void CBackend::underline(std::string line, Span s) {
    int fc = first_char(line);
    printf("%s", std::string(fc, ' ').c_str());
    for (int i = fc; i < line.size(); i++) {
	if (i >= s.column) {
	    while(i <= s.offset) {
		printf("^");
		i++;
	    }
	}
    }
    printf("\n");
}

void CBackend::format_redefinition(Redefinition err) {
    auto second_line = source.at(err.second.line-1);
    printf("Error: %s:%d:%d-%d: Redefinition of '%s'\n",
    err.second.filename.c_str(), err.second.line, err.second.column, err.second.offset, err.symbol_name.c_str());
    printf(" %1c |\n", ' ');
    printf(" %1d |%s\n", err.second.line, second_line.c_str());
    printf(" %1c |", ' ');

    underline(second_line, err.second);
    
    auto first_line = source.at(err.first.line-1);
    printf("-----> %s:%d:%d-%d: First Defined Here\n",
    err.first.filename.c_str(), err.first.line, err.first.column, err.first.offset);
    printf(" %1c |\n", ' ');
    printf(" %1d |%s\n", err.first.line, first_line.c_str());
    printf(" %1c |", ' ');

    underline(first_line, err.first);
}

void CBackend::flush() {
    for (auto& e: errors) {
	if (is<TypeMisMatch>(e)) {
	    TypeMisMatch err = to(TypeMisMatch, e);
	    format_typemismatch(err);
	}
	else if (is<Redefinition>(e)) {
	    Redefinition err = to(Redefinition, e);
	    format_redefinition(err);
	}
    }
}
