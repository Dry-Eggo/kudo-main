#include "defines.hpp"
#include "nodes.hpp"
#include <cbackend.hpp>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;


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

void CBackend::parse() {

    const char* header_path = getenv("KUDO_HEADER");
    if (!header_path) {
      printf("Kudo: Error: Environment Not Set\n");
      printf("     * Missing KUDO_HEADER\n");
      exit(1);
    }
    this->header_path = header_path;
    auto cwd = fs::current_path();
    auto build_dirpath = std::format("{}/.build", cwd.string());
    if (fs::exists(build_dirpath)) {
        if (fs::is_directory(build_dirpath)) {
           this->build_directory = build_dirpath; 
        } else {
            printf("Kudo: Error: '%s' found but is not a directory", build_dirpath.c_str());
            printf("Kudo requires this directory to store artifacts\n");
            exit(1);
        }
    } else {
        fs::create_directory(build_dirpath);
        this->build_directory = build_dirpath;
    }

    printf("Kudo: Artifacts folder: '%s'\n", this->build_directory.c_str());
    // include the runtime header for kudo
    includes = std::format("#include <{}>\n", this->header_path);    
    // Initialize Global Context and another context for nesting
    global_context =  mk_s(VarMap, nullptr);
    current_context = global_context;    
    // Top Level Parser
    for (StmtPtr &stmt : program) {
	if (is<uniq(FunctionDef)>(stmt->data)) {
	    auto func = as(uniq(FunctionDef), stmt);
	    auto code = gen_function(func);
	    body += code.code;
	}
	else if (is<uniq(ExternDef)>(stmt->data)) {
	    auto extrn = as(uniq(ExternDef), stmt);
	    if (extrn->kind == ExternDef::ExternType::Function) {
		auto func = as(uniq(FunctionDef), extrn->stmt);
		auto func_code = gen_function(func);
		out_header += format("extern {}", func_code.code);
	    }
	}
    }
    if (!errors.empty()) {
	flush();
	exit(1);
    }
    output += includes;
    output += out_header;
    output += body;
}

void CBackend::new_ctx() {
    auto nctx = mk_s(VarMap, current_context);
    current_context = nctx;
}

void CBackend::close_ctx() {
    if (current_context->parent) current_context = current_context->parent;
    else current_context = global_context;
}

std::string CBackend::spread_params(Params params, bool is_variadic) {
    std::string code;
    int n = 0;
    for (auto p: params) {
	code += format("{} {}", type_torepr(type_tostr(p.type.get())), p.name);
	
	if (n == params.size() - 1) break;
	code += ", ";
	n++;
    }
    if (is_variadic) code += ", ...";
    return code;
}

CResult CBackend::gen_function(FunctionDef *func) {
    std::string code;
    std::string type_str = type_tostr(func->return_type.get());
    code += std::format("\n{} {} ({})\n", type_str, func->name, spread_params(func->params, func->is_variadic));
    if (func->is_definition) {
	code += std::format("{{\n");
	new_ctx();
	for (auto param: func->params) {
	    current_context->add_var(param.name, mk_s(VariableMeta, param.name, param.type, param.span, SymbolKind::Variable));
	}
	auto body_expr = as(uniq(ExprStmt), func->body);
	code += gen_expr(body_expr->expr.get()).code;
	close_ctx();
	code += std::format("}}\n");
    } else {
	code += ";";
    }

    functions.add_func(func->name, mk_s(FunctionMeta, func->name, func->return_type, func->span, func->params, func->is_variadic));
    current_context->add_var(func->name, mk_s(VariableMeta, func->name, func->return_type, func->span, SymbolKind::Function));    
    return CResult(code, "", NULL);
}

CResult CBackend::gen_stmt(Stmt* stmt) {
    std::string code;
    if (is<uniq(VariableDecl)>(stmt->data)) {
	auto var_decl = as(uniq(VariableDecl), stmt);
	auto value    = gen_expr(var_decl->expr.get());

	if (current_context->has_var(var_decl->name)) {
	    auto var_info = current_context->get_var(var_decl->name);
	    add_error(Redefinition(var_info->declaration, stmt->span, var_decl->name));
	    return CResult("", "", var_info->type);
	}
	
	std::string type_str;
	if (var_decl->type) {
	    auto type_repr = type_tostr(var_decl->type.get());	   
	    auto type_info = types.get_type(type_repr);
	    type_str = type_tostr(value.type.get());
	    if (!type_match(type_info, value.type)) {
		auto got = type_str;
		add_error(TypeMisMatch(stmt->span, type_repr, got));
	    }
	}
	
	code += std::format("{} {} = {}", type_str, var_decl->name, value.code);
	current_context->add_var(var_decl->name, mk_s(VariableMeta, var_decl->name, var_decl->type, stmt->span, SymbolKind::Variable));
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
	if (!current_context->has_var(ident->value)) {
	    add_error(Undeclared(expr->span, ident->value));
	}
	code = ident->value;
    }
    else if (is<uniq(BinaryOp)>(expr->data)) {

	auto binop = as(uniq(BinaryOp), expr);
	auto op = binop->op;

	auto lhs_code = gen_expr(binop->lhs.get());
	auto rhs_code = gen_expr(binop->rhs.get());

	// if (!type_match(lhs_code.type, rhs_code.type)) {
	//     // todo: error
	//     goto recover;
	// }

	if (op == BinaryOp::BinOp::Add) code = format("({}) + ({})", lhs_code.code, rhs_code.code);
	else if (op == BinaryOp::BinOp::Sub) code = format("({}) - ({})", lhs_code.code, rhs_code.code);
	type = types.get_type("int");
	
    }
    else if (is<uniq(FunctionCall)>(expr->data)) {

	auto fc   = as(uniq(FunctionCall), expr);
	std::string ident_expr;
	shr(FunctionMeta) func_info;
	
	if (is<uniq(IdentN)>(fc->callee->data)) {
	    ident_expr = gen_expr(fc->callee.get()).code;
	    if (!functions.has_func(ident_expr)) {
		add_error(UndeclaredFunction(expr->span, ident_expr));
		goto recover;
	    }
	    func_info = functions.get_func(ident_expr);
	}	

	auto func_info_param_count = func_info->params.size();
	if (func_info->variadic) {
	    func_info_param_count = fc->args.size();
	}
	
	if (func_info_param_count != fc->args.size()) {
	    add_error(InvalidParameterCount(expr->span, func_info->declaration, func_info->params.size(), fc->args.size()));
	    goto recover;
	}
	
	std::string args;	
	int n = 0;
	for (auto& arg: fc->args) {
	    auto arg_code = gen_expr(arg.get());
	    
	    args += arg_code.code;
	    if (n == fc->args.size() - 1) break;
	    args += ",";
	    n++;
	}
	code = std::format("{}({})", ident_expr, args);
    }
recover:
    return CResult(code, "", type);
}

std::string CBackend::type_tostr(Type* type) {
    switch(type->type)
    {
	case Types::BaseType::Int: return "int";
	case Types::BaseType::CString: return "cstr";
	case Types::BaseType::String:  return "kudo_string";
	case Types::BaseType::Void:    return "void";
	default: return "error_type";
    }
}

std::string CBackend::type_torepr(std::string key) {
    auto t = types.get_type(key);
    return t->repr;
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
    printf(" %c |\n", ' ');
    printf(" %d |%s\n", err.span.line, line.c_str());
    printf(" %c |", ' ');
    
    underline(line, err.span);
}

void CBackend::underline(std::string line, Span s) {
    int fc = first_char(line);
    printf("%s", std::string(fc, ' ').c_str());
    int i = fc;
    while (i < s.column - 1) {
	printf(" ");
	i++;
    }
    if (i >= s.column - 1) {
	while(i <= s.offset - 1) {
	    printf("^");
	    i++;
	}
    }
    
    printf("\n");
}

void CBackend::format_redefinition(Redefinition err) {
    auto second_line = source.at(err.second.line-1);
    printf("Error: %s:%d:%d-%d: Redefinition of '%s'\n",
    err.second.filename.c_str(), err.second.line, err.second.column, err.second.offset, err.symbol_name.c_str());
    printf(" %5c |\n", ' ');
    printf(" %5d |%s\n", err.second.line, second_line.c_str());
    printf(" %5c |", ' ');

    underline(second_line, err.second);
    
    auto first_line = source.at(err.first.line-1);
    printf("--> %s:%d:%d-%d: First Defined Here\n",
    err.first.filename.c_str(), err.first.line, err.first.column, err.first.offset);
    printf(" %5c |\n", ' ');
    printf(" %5d |%s\n", err.first.line, first_line.c_str());
    printf(" %5c |", ' ');

    underline(first_line, err.first);
}

void CBackend::format_undeclared(Undeclared err) {
    auto line = source.at(err.span.line-1);
    printf("Error: %s:%d:%d-%d: '%s' was not declared in this scope\n", err.span.filename.c_str(), err.span.line, err.span.column, err.span.offset, err.symbol_name.c_str());
    printf(" %5c |\n", ' ');
    printf(" %5d |%s\n", err.span.line, line.c_str());
    printf(" %5c |", ' ');
    underline(line, err.span);
}

void CBackend::format_undeclaredf(UndeclaredFunction err) {
    auto line = source.at(err.span.line-1);
    printf("Error: %s:%d:%d-%d: Implicit declaration of '%s' is not allowed\n", err.span.filename.c_str(), err.span.line, err.span.column, err.span.offset, err.symbol_name.c_str());
    printf(" %5c |\n", ' ');
    printf(" %5d |%s\n", err.span.line, line.c_str());
    printf(" %5c |", ' ');
    underline(line, err.span);
}

void CBackend::format_invpc(Semantic::InvalidParameterCount err) {
    auto line = source.at(err.callee.line-1);
    printf("Error: %s:%d:%d-%d: Invalid parameter count. %d was expected but %d was provided\n", err.callee.filename.c_str(), err.callee.line, err.callee.column,
    err.callee.offset, err.expected, err.got);
    printf(" %5c |\n", ' ');
    printf(" %5d |%s\n", err.callee.line, line.c_str());
    printf(" %5c |", ' ');
    underline(line, err.callee);    
}

void CBackend::flush(bool safe) {
    for (auto& e: errors) {
	if (is<TypeMisMatch>(e)) {
	    TypeMisMatch err = to(TypeMisMatch, e);
	    format_typemismatch(err);
	}
	else if (is<Redefinition>(e)) {
	    Redefinition err = to(Redefinition, e);
	    format_redefinition(err);
	}
	else if (is<Undeclared>(e)) {
	    Undeclared err = to(Undeclared, e);
	    format_undeclared(err);
	}
	else if (is<UndeclaredFunction>(e)) {
	    UndeclaredFunction err = to(UndeclaredFunction, e);
	    format_undeclaredf(err);
	}
	else if (is<InvalidParameterCount>(e)) {
	    InvalidParameterCount err = to(InvalidParameterCount, e);
	    format_invpc(err);
	}
    }
    if (!safe && !errors.empty()) {
	exit(1);
    }
}
