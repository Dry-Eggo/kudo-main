
#include <CLI11.hpp>
#include <lexer.hpp>
#include <parser.hpp>
#include <cbackend.hpp>
#include <fstream>
#include <filesystem>

using Kudo::Language::Lexer;
using Kudo::Language::Parser;
using Kudo::CodeGen::CBackend;

std::string filestem(std::string path) {
    char* c_str = strdup(std::filesystem::path(path).filename().string().data());
    const char* dot = strrchr(c_str, '.');
    auto dotn = dot - c_str;
    c_str[dotn] = '\0';
    return c_str;
}

std::string with_ext(std::string path, std::string ext) {
    char* c_str = path.data();
    const char* dot = strrchr(c_str, '.');
    auto dotn = dot - c_str;
    c_str[dotn] = '\0';
    return std::format("{}.{}", c_str, ext);
}

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
  CBackend cbackend(mv(parser.program), parser.error.source);
  cbackend.parse();

  std::string output = with_ext(input_file, "c");
  std::ofstream of = std::ofstream(output);
  of << cbackend.output;
  of.close();

  system(format("clang {} -o {}", output, filestem(input_file)).c_str());
}
