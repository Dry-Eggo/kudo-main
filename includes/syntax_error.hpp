#pragma once
#include "token.hpp"
#include <format>
#include <iostream>
#include <sstream>
#include <vector>

namespace Kudo::Error::Syntax {
using Kudo::Language::Span;
struct Diagnostic {
  Span span;
  std::string message;
  std::string hint;
  Diagnostic(Span span, std::string msg, std::string h)
      : span(span), message(msg), hint(h) {}
};

struct SyntaxErrorEngine {
  std::vector<std::string> source;
  std::vector<Diagnostic> errors;

  SyntaxErrorEngine(std::string src) {
    std::stringstream s;
    s << src;
    std::string line;
    while (std::getline(s, line))
      source.push_back(line);
  }

  void add(Diagnostic d) { errors.push_back(d); }

  void report(Diagnostic d, bool exit_) {
    auto span = d.span;
    // Line is 1-based but indexing is 0-based
    auto line = source.at(span.line - 1);
    std::cout << std::format("Kudo: Syntax Error {}:({}:{}): {}\n",
                             span.filename, span.line, span.column, d.message);
    std::cout << std::format("{} | {}\n ", span.line, line);
    for (auto c : line) {
      if (isspace(c)) {
        std::cout << " ";
      }
    }
    for (int i = 0; i <= span.offset; i++) {
      if (i == span.offset) {
        std::cout << "^";
      } else
        std::cout << " ";
    }
    std::cout << "\n";
    ;
    if (exit_)
      exit(1);
  }

  void flush() {
    if (!errors.empty()) {
      for (auto d : errors) {
        report(d, false);
      }
      exit(1);
    }
  }
};
} // namespace Kudo::Error::Syntax
