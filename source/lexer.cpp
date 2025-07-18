#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <lexer.hpp>
#include <sstream>

namespace fs = std::filesystem;

namespace Kudo {
    namespace Language {
	Lexer::Lexer(std::string filename) {
	    this->filename = filename;
	    if (!fs::exists(filename)) {
		std::cout << "Unable to open '" << filename
		<< "' : No Such file or directory\n";
		exit(1);
	    }
	    std::fstream f = std::fstream(filename, std::ios::in);
	    std::stringstream s;
	    s << f.rdbuf();
	    source = s.str();
	}
	char Lexer::now() {
	    if (pos >= source.size()) {
		return EOF;
	    }
	    return source.at(pos);
	}
	char Lexer::peek() {
	    if (pos + 1 >= source.size()) {
		return EOF;
	    }
	    return source.at(pos + 1);
	}
	char Lexer::before() {
	    if (pos - 1 < 0) {
		return EOF;
	    }
	    return source.at(pos - 1);
	}
	char Lexer::advance() {
	    char ch = now();
	    if (ch == '\n') {
		line++;
		column = 1;
	    } else {
		column++;
	    }
	    pos++;
	    return ch;
	}
	
	Tokens Lexer::parse() {
	    Tokens tokens;
	    while (now() != EOF) {
		if (isspace(now())) {
		    advance();
		    continue;
		}

		if (now() == 'c' && peek() == '"') {
		    advance(); advance();
		    std::string buf;
		    int sc = column;
		    while (now() != EOF && now() != '"') {
			buf += advance();
		    }
		    advance();
		    tokens.push_back(Token(TokenKind::CSTRING, buf, Span(filename.c_str(), line, sc, column - 1)));
		}
		
		if (isalpha(now()) || now() == '_') {
		    std::string buf;
		    int sc = column;
		    while (now() != EOF && (isalnum(now()) || now() == '_')) {
			buf += advance();
		    }
		    
		    // TODO: add all keywords
		    if (buf == "end" || buf == "func" || buf == "import" || buf == "if" || buf == "int" || buf == "str" ||
		    buf == "var" || buf == "if" || buf == "elif" || buf == "else" || buf == "loop" || buf == "false" || buf == "true" ||  buf == "cstr") {
			tokens.push_back(Token(TokenKind::KEYWORD, buf,
                        Span(filename.c_str(), line, sc, column - 1)));
			continue;
		    }
		    tokens.push_back(Token(TokenKind::NAME, buf,
                    Span(filename.c_str(), line, sc, column - 1)));
		    continue;
		}
		if (isdigit(now())) {
		    std::string buf;
		    int sc = column;
		    while (now() != EOF && isdigit(now())) {
			buf += advance();
		    }
		    tokens.push_back(Token(TokenKind::NUMBER, buf,
                    Span(filename.c_str(), line, sc, column - 1)));
		    continue;
		}
		if (now() == '\"') {
		    advance();
		    std::string buf;
		    int sc = column;
		    while (now() != '\"') {
			buf += advance();
		    }
		    advance();
		    tokens.push_back(Token(TokenKind::STRING, buf,
                    Span(filename.c_str(), line, sc, column - 1)));
		    continue;
		}
		
		int sc = column;
		switch (now()) {
		case '=':
		    advance();
		    if (now() == '=') {
			advance();
			tokens.push_back(Token(TokenKind::EQEQ, "==",
			Span(filename.c_str(), line, sc, column - 1)));
			break;
		    }
		    tokens.push_back(Token(TokenKind::EQ, "=",
                    Span(filename.c_str(), line, sc, column - 1)));
		    break;
		case '!':
		    advance();
		    if (now() == '=') {
			advance();
			tokens.push_back(Token(TokenKind::NEQ, "!=",
			Span(filename.c_str(), line, sc, column - 1)));
			break;
		    }
		    tokens.push_back(Token(TokenKind::BANG, "*",
                    Span(filename.c_str(), line, sc, column - 1)));
		    break;
		case '+':
		    advance();
		    tokens.push_back(Token(TokenKind::ADD, "+",
                    Span(filename.c_str(), line, sc, column - 1)));
		    break;
		case '-':
		    advance();
		    tokens.push_back(Token(TokenKind::SUB, "-",
                    Span(filename.c_str(), line, sc, column - 1)));
		    break;
		case '*':
		    advance();
		    tokens.push_back(Token(TokenKind::MUL, "*",
                    Span(filename.c_str(), line, sc, column - 1)));
		    break;
		case '/':
		    advance();
		    if (now() == '/') {
			advance();
			while (now() != '\n') advance();
			continue;
		    }
		    tokens.push_back(Token(TokenKind::DIV, "/",
                    Span(filename.c_str(), line, sc, column - 1)));
		    break;
		case '|':
		    advance();
		    if (now() == '|') {
			advance();
			tokens.push_back(Token(TokenKind::OR, "||",
			Span(filename.c_str(), line, sc, column - 1)));
			break;
		    }
		    tokens.push_back(Token(TokenKind::BOR, "|",
                    Span(filename.c_str(), line, sc, column - 1)));
		case '&':
		    advance();
		    if (now() == '&') {
			advance();
			tokens.push_back(Token(TokenKind::AND, "&&",
			Span(filename.c_str(), line, sc, column - 1)));
			break;
		    }
		    tokens.push_back(Token(TokenKind::BAND, "&",
                    Span(filename.c_str(), line, sc, column - 1)));
		case ':':
		    advance();
		    tokens.push_back(Token(TokenKind::COLON, ":",
                    Span(filename.c_str(), line, sc, column - 1)));
		    break;
		case ',':
		    advance();
		    tokens.push_back(Token(TokenKind::COMMA, ".",
                    Span(filename.c_str(), line, sc, column - 1)));
		    break;
		case '(':
		    advance();
		    tokens.push_back(Token(TokenKind::OPEN_PAREN, "(",
                    Span(filename.c_str(), line, sc, column - 1)));
		    break;
		case ')':
		    advance();
		    tokens.push_back(Token(TokenKind::CLOSE_PAREN, ")",
                    Span(filename.c_str(), line, sc, column - 1)));
		    break;
		case '[':
		    advance();
		    tokens.push_back(Token(TokenKind::OPEN_BRACKET, "[",
                    Span(filename.c_str(), line, sc, column - 1)));
		    break;
		case ']':
		    advance();
		    tokens.push_back(Token(TokenKind::CLOSE_BRACKET, "]",
                    Span(filename.c_str(), line, sc, column - 1)));
		    break;
		case '{':
		    advance();
		    tokens.push_back(Token(TokenKind::OPEN_BRACE, "{",
                    Span(filename.c_str(), line, sc, column - 1)));
		    break;
		case '}':
		    advance();
		    tokens.push_back(Token(TokenKind::CLOSE_BRACE, "}",
                    Span(filename.c_str(), line, sc, column - 1)));
		    break;
		case '@':
		    advance();
		    tokens.push_back(Token(TokenKind::AT, "@",
                    Span(filename.c_str(), line, sc, column - 1)));
		    break;
		case '$':
		    advance();
		    tokens.push_back(Token(TokenKind::DOLA, "$",
                    Span(filename.c_str(), line, sc, column - 1)));
		    break;
		case ';':
		    advance();
		    tokens.push_back(Token(TokenKind::SEMI, ";",
                    Span(filename.c_str(), line, sc, column - 1)));
		    break;
		default:
		    std::stringstream s;
		    s << "Unexpected char: '" << now() << "'\n";
		    std::cout << s.str() << std::endl;
		    exit(1);
		}
	    }
	    tokens.push_back(Token(TokenKind::TEOF, "EOF",
            Span(filename.c_str(), line, column, column)));
	    return tokens;
	}
    } // namespace Language
} // namespace Tisp
