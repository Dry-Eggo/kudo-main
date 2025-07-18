#pragma once
#include <string>
#include <vector>
#include <string.h>
#include <token.hpp>

namespace Kudo {
    namespace Language {
	
	typedef std::vector<Token> Tokens;
	
	struct Lexer {
	    std::string filename;
	    std::string source;
	    
	    int         pos    = 0;
	    int         column = 1;
	    int         line   = 1;
	    
	    Lexer(std::string filename);
	    char now();
	    char advance();
	    char before();
	    char peek();
	    Tokens parse();
	};
    } // Language
} // Kudo
