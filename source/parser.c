#include <parser.h>
#include <globals.h>


#define neof()          (now(parser).kind != TOKEN_EOS)
#define match(tok)      (now(parser).kind == tok)
#define advance()       parser->cursor++;

Token peek(Parser* parser);
Token now(Parser* parser);
Token before(Parser* parser);
Token look_ahead(Parser* parser, size_t n);

Expr* parse_expr(Parser* parser);
Expr* parse_atom(Parser* parser);
Expr* parse_body(Parser* parser);

Stmt* parse_function(Parser* parser);

void expect(Parser* parser, TokenKind k) {
    if (!match(k)) {
	Token tok = now(parser);
	elog("Unexpected token: %s:%ld:%ld: '%s'\n", tok.span.filename, tok.span.line, tok.span.column, tok.lexme);
	exit(1);
    }
    advance();
}

void error(Parser* parser, const char* msg) {
	Token tok = now(parser);
	elog("Error: %s:%ld:%ld: '%s'\n", tok.span.filename, tok.span.line, tok.span.column, msg);
	exit(1);
}

Token peek(Parser* parser) {
    if (parser->cursor + 1 >= parser->tokens->count) {
	// EOS
	return tl_get(parser->tokens, parser->tokens->count - 1);
    }
    return tl_get(parser->tokens, parser->cursor + 1);
}

Token now(Parser* parser) {
    if (parser->cursor >= parser->tokens->count) {
	return tl_get(parser->tokens, parser->tokens->count - 1);
    }
    return tl_get(parser->tokens, parser->cursor);
}

Token before(Parser* parser) {
    if (parser->cursor - 1 <= 0) {
	return tl_get(parser->tokens, parser->tokens->count - 1);
    }
    return tl_get(parser->tokens, parser->cursor - 1);
}

Token look_ahead(Parser* parser, size_t n) {
    if (parser->cursor + n >= parser->tokens->count) {
	return tl_get(parser->tokens, parser->tokens->count - 1);
    }
    return tl_get(parser->tokens, parser->cursor + n);
}

Parser* kudo_parser_init(TokenList* tokens, Nob_String_Builder sb) {
    Parser* parser = NEW(Parser);
    parser->source = sb;
    parser->tokens = tokens;
    parser->cursor = 0;
    list_Stmt_init(&parser->parsed_program);
    
    return parser;
}

void kudo_parser_parse(Parser* parser) {
    while (neof()) {
	if (match(TOKEN_FUNC)) {
	    Stmt* func_stmt = parse_function(parser);
	    list_Stmt_add(parser->parsed_program, func_stmt);
	}
    }
}

Stmt* parse_function(Parser* parser) {
    expect(parser, TOKEN_FUNC);
    if (!match(TOKEN_IDENTIFIER)) {
	error(parser, "Expected identifier");
    }
    const char* name = now(parser).lexme;
    advance();
    
    expect(parser, TOKEN_OPAREN);
    expect(parser, TOKEN_CPAREN);

    Type* return_type = create_type(TYPE_INT);
    Expr* body = parse_expr(parser);
    return create_stmt_funcdecl(str_dup(name), NULL, return_type, body);
}

Stmt* parse_vardecl(Parser* parser) {
    expect(parser, TOKEN_VAR);
    if (!match(TOKEN_IDENTIFIER)) {
	error(parser, "Expected identifier");
    }
    const char* name = now(parser).lexme;
    advance();
    
    expect(parser, TOKEN_COLON);
    expect(parser, TOKEN_EQ);

    Expr* value = parse_expr(parser);
    return create_stmt_vardeclauto(str_dup(name), value);
}

Expr* parse_body(Parser* parser) {
    list_Stmt* cstmt = {0}; list_Stmt_init(&cstmt);
    expect(parser, TOKEN_OCURLY);

    while(!match(TOKEN_CCURLY)) {
	if (match(TOKEN_VAR)) {
	    list_Stmt_add(cstmt, parse_vardecl(parser));
	}
	expect(parser, TOKEN_SEMI);
    }
    
    expect(parser, TOKEN_CCURLY);
    return create_stmt_compound(cstmt);
}

Expr* parse_expr(Parser* parser) {
    Expr* expr = parse_atom(parser);
    return expr;
}

Expr* parse_atom(Parser* parser) {
    switch(now(parser).kind)
    {
	case TOKEN_INTEGER: {
	    int i = strtol(now(parser).lexme, NULL, 10);
	    advance();
	    return create_expr_int(i);
	} break;
	case TOKEN_OCURLY: {
	    Expr* body = parse_body(parser);
	    return body;
	} break;
    default:
	error(parser, "Unexpected Token");
	exit(1);
    }
}
