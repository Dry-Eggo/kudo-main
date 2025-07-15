#include "lexer.h"
#include "globals.h"
#include "string_builder.h"
#include "token.h"
#include "enum.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



Lexer* kudo_lexer_init(const char* input_path, Nob_String_Builder source) {
	Lexer* l = arena_alloc(main_arena, sizeof(Lexer));
	l->position = 0;
	l->col = 1;
	l->line = 1;
	l->source = source;
	l->tokens = tl_init();
	l->input_path = input_path;
	return l;
}


// Traversal helpers
static char kudo_lexer_now(Lexer *lexer) {
	if (lexer->position < lexer->source.count)
		return lexer->source.items[lexer->position];
	return '\0';
}

static char kudo_lexer_peek(Lexer *lexer, size_t offset) {
	size_t pos = lexer->position + offset;
	if (pos < lexer->source.count)
		return lexer->source.items[pos];
	return '\0';
}

static char kudo_lexer_advance(Lexer *lexer) {
	char c = kudo_lexer_now(lexer);
	if (c == '\n') {
		lexer->line++;
		lexer->col = 1;
	} else {
		lexer->col++;
	}
	lexer->position++;
	return c;
}

static void kudo_lexer_add_token_from_buffer(Lexer *lexer, TokenKind kind, Nob_String_Builder *buffer) {
	Span span = create_span(NULL, lexer->line, lexer->col - (int)buffer->count, lexer->col - 1);
	Token token = create_token(kind, buffer->items, span); // buffer->data is null-terminated
	tl_add(lexer->tokens, token);
}

// Skip whitespace
static void kudo_lexer_skip_whitespace(Lexer *lexer) {
	while (isspace(kudo_lexer_now(lexer))) {
		kudo_lexer_advance(lexer);
	}
}

void kudo_lexer_lex(Lexer *lexer) {
	while (kudo_lexer_now(lexer) != '\0') {
		if (isspace(kudo_lexer_now(lexer))) {
			kudo_lexer_advance(lexer);
			continue;
		}

		if (isalpha(kudo_lexer_now(lexer)) || kudo_lexer_now(lexer) == '_') {
			StringBuilder* buffer;
			SB_init(&buffer);
			size_t line = lexer->line;
			size_t col = lexer->col;

			while (isalnum(kudo_lexer_now(lexer)) || kudo_lexer_now(lexer) == '_') {
				sbapp(buffer, "%c", kudo_lexer_now(lexer));
				kudo_lexer_advance(lexer);
			}
			if (sbeq(buffer, "func")) {
				tl_add(lexer->tokens, create_token(TOKEN_FUNC, arena_strdup(main_arena, buffer->data), create_span(lexer->input_path, line, col, lexer->col-1)));
			} else if (sbeq(buffer, "var")) {
				tl_add(lexer->tokens, create_token(TOKEN_VAR, arena_strdup(main_arena, buffer->data), create_span(lexer->input_path, line, col, lexer->col-1)));
			} else {
				tl_add(lexer->tokens, create_token(TOKEN_IDENTIFIER, arena_strdup(main_arena, buffer->data), create_span(lexer->input_path, line, col, lexer->col-1)));
			}
			SB_free(buffer);
			continue;
		}
		
		if (isdigit(kudo_lexer_now(lexer))) {
			StringBuilder* buffer;
			SB_init(&buffer);
			size_t line = lexer->line;
			size_t col = lexer->col;

			while (isdigit(kudo_lexer_now(lexer))) {
				sbapp(buffer, "%c", kudo_lexer_now(lexer));
				kudo_lexer_advance(lexer);
			}
			tl_add(lexer->tokens, create_token(TOKEN_INTEGER, arena_strdup(main_arena, buffer->data), create_span(lexer->input_path, line, col, lexer->col-1)));
			SB_free(buffer);
			continue;
		}

		StringBuilder* buffer;
		SB_init(&buffer);
		size_t line = lexer->line;
		size_t col = lexer->col;
		switch(kudo_lexer_now(lexer)) {			
			case '(': {
				kudo_lexer_advance(lexer);
				tl_add(lexer->tokens, create_token(TOKEN_OPAREN, "(", create_span(lexer->input_path, line, col, lexer->col-1)));
				continue;
			} break;
			case ')': {
				kudo_lexer_advance(lexer);
				tl_add(lexer->tokens, create_token(TOKEN_CPAREN, ")", create_span(lexer->input_path, line, col, lexer->col-1)));
				continue;
			} break;
			case '{': {
				kudo_lexer_advance(lexer);
				tl_add(lexer->tokens, create_token(TOKEN_OCURLY, "{", create_span(lexer->input_path, line, col, lexer->col-1)));
				continue;
			} break;
			case '}': {
				kudo_lexer_advance(lexer);
				tl_add(lexer->tokens, create_token(TOKEN_CCURLY, "}", create_span(lexer->input_path, line, col, lexer->col-1)));
				continue;
			} break;
			case ';': {
				kudo_lexer_advance(lexer);
				tl_add(lexer->tokens, create_token(TOKEN_SEMI, ";", create_span(lexer->input_path, line, col, lexer->col-1)));
				continue;
			} break;
			case ':': {
				kudo_lexer_advance(lexer);
				tl_add(lexer->tokens, create_token(TOKEN_COLON, ":", create_span(lexer->input_path, line, col, lexer->col-1)));
				continue;
			case '=': {
				kudo_lexer_advance(lexer);
				tl_add(lexer->tokens, create_token(TOKEN_EQ, ";", create_span(lexer->input_path, line, col, lexer->col-1)));
				continue;
			} break;
			} break;
			default:
				elog("Unknown Char: '%c'\n", kudo_lexer_now(lexer));
				exit(1);
		}
	}
	tl_add(lexer->tokens, create_token(TOKEN_EOS, "<eos>", create_span(lexer->input_path, lexer->line, lexer->col, lexer->col)));
}
