#pragma once
#include <string>
#include <unordered_map>

namespace Kudo::Language {

enum class TokenKind {
  NAME,
  KEYWORD,
  NUMBER,
  STRING,
  CSTRING,
  
  // punctuations
  OPEN_PAREN,
  CLOSE_PAREN,
  OPEN_BRACKET,
  CLOSE_BRACKET,
  OPEN_BRACE,
  CLOSE_BRACE,
  COMMA,
  COLON,
  SEMI,
  DOT,
  // operators
  EQ,    // =
  BANG,  // !
  EQEQ,  // ==
  NEQ,   // !=
  ADD,   // +
  SUB,   // -
  MUL,   // *
  DIV,   // /
  SHR,   // >>
  SHL,   // <<
  ADDEQ, // +=
  SUBEQ, // -=
  MULEQ, // *=
  DIVEQ, // /=
  INC,   // ++
  DEC,   // --
  OR,    // ||
  AND,   // &&
  BAND,  // &
  BOR,   // |
  DOLA,  // $
  AT,    // @
  TEOF,
};
static std::string tokenkind_tostr(TokenKind k) {
  static std::unordered_map<TokenKind, std::string> tmap = {
      {TokenKind::STRING, "String"},  {TokenKind::NUMBER, "Number"},
      {TokenKind::OPEN_BRACE, "{"},   {TokenKind::CLOSE_BRACE, "}"},
      {TokenKind::OPEN_BRACKET, "["}, {TokenKind::CLOSE_BRACKET, "]"},
      {TokenKind::OPEN_PAREN, "("},   {TokenKind::CLOSE_PAREN, ")"}};
  return tmap[k];
}
struct Span {
  int line;
  int column;
  int offset;
  std::string filename;

  Span(std::string filename, int line, int column, int offset)
  : filename(filename), line(line), column(column), offset(offset) {}

  Span merge(Span s) {
      offset = s.offset;
      return *this;
  }
};

struct Token {
  TokenKind kind;
  std::string data;
  Span span;
  Token(TokenKind k, std::string lexme, Span s)
      : kind(k), data(lexme), span(s) {}
};

} // namespace Kudo::Language
