#include "syntax_error.hpp"
#include "token.hpp"
#include <defines.hpp>
#include <parser.hpp>
#include <string>
#include <vector>

using Kudo::Error::Syntax::Diagnostic;
using Kudo::Language::ExprPtr;
using Kudo::Language::Parser;
using Kudo::Language::StmtPtr;
using Kudo::Language::Token;

Token Parser::peek() {
    if (cursor + 1 >= tokens.size()) {
	return tokens.back();
    }
    return tokens.at(cursor + 1);
}

Token Parser::now() {
    if (cursor >= tokens.size()) {
	return tokens.back();
    }
    return tokens.at(cursor);
}

Token Parser::before() {
    if (cursor - 1 <= 0) {
	return tokens.back();
    }
    return tokens.at(cursor);
}

bool Parser::match(TokenKind kind) { return (now().kind == kind); }
bool Parser::match(std::string kw) {
    return (now().kind == TokenKind::KEYWORD && now().data == kw);
}

void Parser::expect(TokenKind kind) {
    if (match(kind)) {
	advance();
	return;
    }
    error.report(
    Diagnostic(now().span,
    "Unexpected Token. Expected '" + tokenkind_tostr(kind) + "'",
    ""),
    true);
}

void Parser::advance() { cursor++; }

StmtPtr Parser::parse_stmt() {
    StmtPtr stmt;
    if (now().kind == TokenKind::KEYWORD) {
	if (now().data == "var") {
	    Span start = now().span;
	    advance();
	    std::string var_name;
	    if (match(TokenKind::NAME)) {
		var_name = now().data;
		advance();
	    }
	    expect(TokenKind::COLON);
	    shr(Type) type = NULL;
	    if (!match(TokenKind::EQ)) {
		type = parse_type();
	    }
	    expect(TokenKind::EQ);
	    auto value = parse_expr();
	    
	    Span end = now().span;
	    Span span = start.merge(end);
	    if (type) stmt = mk_u(Stmt, mv(mk_u(VariableDecl, var_name, mv(value), type)), span);
	    else stmt = mk_u(Stmt, mv(mk_u(VariableDecl, var_name, mv(value))), span);
	}
    } else {
	Span start = now().span;
	auto expr = parse_expr();
	Span end = now().span;
	
	stmt = mk_u(Stmt, mv(mk_u(ExprStmt, mv(expr))), start.merge(end));
    }
    expect(TokenKind::SEMI);
    return stmt;
}

shr(Type) Parser::parse_type() {
    switch (now().kind)
    {
	case TokenKind::KEYWORD: {
	    if (now().data == "int") {
		advance();
		return Type::new_type(BaseType::Int);
	    } else if (now().data == "str") {
		advance();
		return Type::new_type(BaseType::String);
	    } else if (now().data == "cstr") {
		advance();
		return Type::new_type(BaseType::CString);		
	    }
	}
    default:
	error.report(Diagnostic(now().span, "Not a valid Type", ""), true);
	exit(1);
    }
}

void Parser::parse() {
    while (!match(TokenKind::TEOF)) {
	switch (now().kind) {
	case TokenKind::KEYWORD: {
	    if (now().data == "func") {
		auto func = parse_function();
		program.push_back(mv(func));
	    }
	    else if (now().data == "extern") {
		auto extrn = parse_extern();
		program.push_back(mv(extrn));
	    }
	} break;
    default:
	auto stmt = parse_stmt();
	program.push_back(mv(stmt));
    }
}
error.flush();
}

StmtPtr Parser::parse_extern() {
    Span start = now().span;
    advance();
    ExternDef::ExternType t = ExternDef::ExternType::Function;
    if (match("func")) {
	auto func = parse_function();
	auto span = func->span;
	return mk_u(Stmt, mv(mk_u(ExternDef, t, mv(func))), span);
    }
    error.report(Diagnostic(now().span, "Invalid external storage candidate", ""), true);
    exit(1); /* shouldn't reach here */
}

StmtPtr Parser::parse_function() {
    Span start = now().span;
    advance();
    std::string function_name;
    Span name_span = now().span;
    if (match(TokenKind::NAME)) {
	function_name = now().data;
	advance();
    }

    expect(TokenKind::OPEN_PAREN);
    Params params;
    bool   is_variadic = false;
    while (!match(TokenKind::CLOSE_PAREN)) {
	Span pstart = now().span;
	if (match(TokenKind::SPREAD)) {
	    advance();
	    is_variadic = true;
	    break;
	}
	
	Token tok = now();
	if (tok.kind != TokenKind::NAME) {
	    error.report(Diagnostic(now().span, "Expected an identifier", ""), true);
	}
	advance();
	expect(TokenKind::COLON);
	auto param_type = parse_type();
	Span pend = now().span;
	params.push_back(Param(tok.data, param_type, pstart.merge(pend)));

	if (match(TokenKind::COMMA)) {
	    advance();
	}
    }
    expect(TokenKind::CLOSE_PAREN);

    shr(Type) ret_type = Type::new_type(BaseType::Void);
    if (match(TokenKind::COLON)) {
	expect(TokenKind::COLON);
	ret_type = parse_type();
    }

    uniq(FunctionDef) function = nullptr;
    if (!match(TokenKind::SEMI)) {
	auto expr = parse_expr();
	auto expr_span = expr->span;
	
	auto body_expr = mk_u(ExprStmt, mv(expr));
	auto body_stmt = mk_u(Stmt, mv(body_expr), expr_span);
	function = mk_u(FunctionDef, function_name, params, mv(body_stmt), ret_type, name_span, is_variadic);
    } else {
	function = mk_u(FunctionDef, function_name, params, ret_type, name_span, is_variadic);
	expect(TokenKind::SEMI);
    }
    
    Span end = now().span;
    Span span = start.merge(end);
    return mk_u(Stmt, mv(function), span);
}

ExprPtr Parser::parse_expr() { return parse_logical_or(); }

ExprPtr Parser::parse_logical_or() {
    auto lhs = parse_logical_and();
    return lhs;
}

ExprPtr Parser::parse_logical_and() {
    auto lhs = parse_equality();
    return lhs;
}

ExprPtr Parser::parse_equality() {
    auto lhs = parse_additive();
    return lhs;
}

ExprPtr Parser::parse_additive() {
    Span start = now().span;
    auto lhs = parse_term();
    while (match(TokenKind::ADD) || match(TokenKind::SUB)) {
	auto op = (now().kind == TokenKind::ADD) ? BinaryOp::BinOp::Add : BinaryOp::BinOp::Sub;
	advance();

	auto rhs = parse_expr();
	Span end = now().span;
	lhs = mk_u(Expr, mv(mk_u(BinaryOp, op, mv(lhs), mv(rhs))), start.merge(end));
    }
    return lhs;
}

ExprPtr Parser::parse_term() {
    auto lhs = parse_postfix();
    return lhs;
}

ExprPtr Parser::parse_postfix() {
    auto base = parse_atom();
    while (match(TokenKind::OPEN_PAREN)) {
	if (match(TokenKind::OPEN_PAREN)) {
	    Span start = base->span;
	    advance();
	    std::vector<ExprPtr> args;
	    while(!match(TokenKind::CLOSE_PAREN)) {
		auto arg = parse_expr();
		args.push_back(mv(arg));

		if (match(TokenKind::COMMA)) advance();		    
	    }
	    expect(TokenKind::CLOSE_PAREN);
	    Span end = now().span;
	    Span span = start.merge(end);
	    base = mk_u(Expr, mv(mk_u(FunctionCall, mv(base), mv(args))), span);
	}
    }
    return base;
}

ExprPtr Parser::parse_atom() {
    switch (now().kind) {
    case TokenKind::OPEN_BRACE: {
	Span start = now().span;
	advance();
	std::vector<StmtPtr> body;
	while (now().kind != TokenKind::CLOSE_BRACE) {
	    auto stmt = parse_stmt();
	    body.push_back(mv(stmt));
	}
	expect(TokenKind::CLOSE_BRACE);

	Span span = start;
	return mk_u(Expr, mv(mk_u(Body, mv(body))), span);
    } break;
    case TokenKind::NUMBER: {
	auto span = now().span;
	auto value = std::stoi(now().data);
	advance();
	return mk_u(Expr, mv(mk_u(Integer, value)), span);
    } break;
    case TokenKind::NAME: {
	auto span = now().span;
	auto value = now().data;
	advance();
	return mk_u(Expr, mv(mk_u(IdentN, value)), span);
    } break;
    case TokenKind::CSTRING: {
	auto span = now().span;
	auto value = now().data;
	advance();
	return mk_u(Expr, mv(mk_u(CStringN, value)), span);
    } break;
default:
    break;
}
error.report(Diagnostic(now().span, "Not a valid expression", ""), true);
exit(1);
}
