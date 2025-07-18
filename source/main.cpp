
#include <CLI11.hpp>
#include <lexer.hpp>
#include <parser.hpp>
#include <cbackend.hpp>

using Kudo::Language::Lexer;
using Kudo::Language::Parser;
using Kudo::CodeGen::CBackend;

int main(int argc, char **argv) {
  CLI::App app{"Kudo Compiler"};

  std::string input_file;
  bool verbose;

  app.add_option("-i, --input", input_file, "Input File")->required();
  app.add_flag("-b, --verbose", verbose, "Enable Verbose Output");

  CLI11_PARSE(app, argc, argv);

  Lexer lexer(input_file);
  auto tokens = lexer.parse();
  if (verbose) {
    for (auto token : tokens) {
      std::cout << "Token " << (int)token.kind << " (" << token.data << ")\n";
    }
  }
  Parser parser(tokens, lexer.source);
  parser.parse();
  std::cout << std::format("Stmt size: {}\n", parser.program.size());
  CBackend cbackend(mv(parser.program));
  cbackend.parse();
}
