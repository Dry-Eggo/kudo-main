#pragma once

#include "defines.hpp"
#include <lexer.hpp>
#include <nodes.hpp>
#include <syntax_error.hpp>

namespace Kudo {
namespace Language {
struct Parser {
  int cursor = 0;
  Tokens tokens;
  std::string source;
  Statements program;
  Error::Syntax::SyntaxErrorEngine error;

  Parser(Tokens &tokens, std::string source) : source(source), error(source) {
    this->tokens = tokens;
  }
  void parse();

  Token now();
  Token peek();
  Token before();
  void advance();

  bool match(TokenKind kind);
  void expect(TokenKind kind);
  Type parse_type();
  StmtPtr parse_function();
  StmtPtr parse_body();
  StmtPtr parse_stmt();
  ExprPtr parse_expr();
  ExprPtr parse_logical_or();
  ExprPtr parse_logical_and();
  ExprPtr parse_equality();
  ExprPtr parse_additive();
  ExprPtr parse_term();
  ExprPtr parse_atom();
};
} // namespace Language
} // namespace Kudo
