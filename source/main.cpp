
#include <CLI11.hpp>
#include <lexer.hpp>
#include <parser.hpp>
#include <cbackend.hpp>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

using Kudo::Language::Lexer;
using Kudo::Language::Parser;
using Kudo::CodeGen::CBackend;

std::string filestem(std::string path) {
    char* c_str = strdup(fs::path(path).filename().stem().string().data());
    return c_str;
}

std::string with_ext(std::string path, std::string ext) {
    return fs::path(path).replace_extension(ext);
}

void run_executable(std::string path) {
    printf("Running: '%s'\n", path.c_str());
    int code = system(format("./{}", path).c_str());
    printf("Program exited with: %d\n", code);
}

int main(int argc, char **argv) {
  CLI::App app{"Kudo Compiler"};

  std::string input_file;
  std::string output_file;
  bool verbose = false;
  bool emitc = false;
  bool runfile = false;
  
  app.add_option("-i, --input",  input_file, "Input File")->required();
  app.add_option("-o, --out",    output_file, "Output path");
  app.add_flag("-r, --run",      runfile, "Run after build");
  app.add_flag("-b, --verbose",  verbose, "Enable Verbose Output");
  app.add_flag("--emit-c",       emitc, "Generate Only C File");

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

  std::string basename = filestem(input_file);
  fs::path    output = fs::path(cbackend.build_directory) / with_ext(basename, "c");
  std::ofstream of = std::ofstream(output);
  of << cbackend.output;
  of.close();

  system(format("clang-format -i {}", output.string()).c_str());
  if (!emitc) {
      if (output_file.empty()) {
	  system(format("clang {} -o {}", output.string(), filestem(input_file)).c_str());
	  if (runfile) run_executable(filestem(input_file));
      } else {
	  system(format("clang {} -o {}", output.string(), output_file).c_str());
	  if (runfile) run_executable(output_file);
      }
  }
}
