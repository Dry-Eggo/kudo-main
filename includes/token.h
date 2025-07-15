#pragma once
#include "enum.h"
#include <stddef.h>

typedef struct {
	const char* filename;
	size_t      line;
	size_t      column;
	size_t      colend;
} Span;

typedef struct {
	TokenKind kind;
	const char* lexme;
	Span span;
} Token;

typedef struct {
	Token* items;
	size_t count;
	size_t cap;
} TokenList;

TokenList* tl_init();
void       tl_add(TokenList* tl, Token t);
Token      tl_get(TokenList* tl, size_t n);
void       tl_free(TokenList* tl);

Span       create_span(const char* filename, size_t line, size_t column, size_t colend);
Token      create_token(TokenKind kind, const char* lexme, Span span);
