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
      advance();
      std::string var_name;
      if (match(TokenKind::NAME)) {
        var_name = now().data;
        advance();
      }
      expect(TokenKind::COLON);
      Type type;
      if (!match(TokenKind::EQ)) {
	  type = parse_type();
      }
      expect(TokenKind::EQ);
      auto value = parse_expr();
      stmt = mk_u(Stmt, mv(mk_u(VariableDecl, var_name, mv(value))));
    }
  } else {
    auto expr = parse_expr();
    stmt = mk_u(Stmt, mv(mk_u(ExprStmt, mv(expr))));
  }
  expect(TokenKind::SEMI);
  return stmt;
}

Type Parser::parse_type() {
    switch (now().kind)
    {
	case TokenKind::KEYWORD: {
	    if (now().data == "int") {
		return Type::new_type(BaseType::Int);
	    } else if (now().data == "str") {
		return Type::new_type(BaseType::String);
	    }
	}
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
    } break;
    default:
      auto stmt = parse_stmt();
      program.push_back(mv(stmt));
    }
  }
  error.flush();
}

StmtPtr Parser::parse_function() {
  advance();
  std::string function_name;
  Params params;
  if (match(TokenKind::NAME)) {
    function_name = now().data;
    advance();
  }

  expect(TokenKind::OPEN_PAREN);
  expect(TokenKind::CLOSE_PAREN);
  auto body_expr = mk_u(ExprStmt, parse_expr());
  auto body_stmt = mk_u(Stmt, mv(body_expr));
  auto function = mk_u(FunctionDef, function_name, params, mv(body_stmt));
  return mk_u(Stmt, mv(function));
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
  auto lhs = parse_term();
  return lhs;
}

ExprPtr Parser::parse_term() {
  auto lhs = parse_atom();
  return lhs;
}

ExprPtr Parser::parse_atom() {
  switch (now().kind) {
  case TokenKind::OPEN_BRACE: {
    advance();
    std::vector<StmtPtr> body;
    while (now().kind != TokenKind::CLOSE_BRACE) {
      auto stmt = parse_stmt();
      body.push_back(mv(stmt));
    }
    expect(TokenKind::CLOSE_BRACE);
    return mk_u(Expr, mv(mk_u(Body, mv(body))));
  } break;
  case TokenKind::NUMBER: {
    auto span = now().span;
    auto value = std::stoi(now().data);
    advance();
    return mk_u(Expr, mv(mk_u(Integer, value)));
  } break;
  default:
    break;
  }
  error.report(Diagnostic(now().span, "Not a valid expression", ""), true);
  exit(1);
}
