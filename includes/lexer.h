#pragma once

#include "nob.h"
#include "options.h"
#include "token.h"
#include <stddef.h>
typedef struct {
	Nob_String_Builder source;
	size_t             position;
	size_t             line;
	size_t             col;

	TokenList          tokens;
} Lexer;

Lexer* kudo_lexer_init(Args* args, Nob_String_Builder source);
void   kudo_lexer_lex(Lexer* lexer);
