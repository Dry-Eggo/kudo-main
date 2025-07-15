#include "token.h"
#include "globals.h"
#include <stddef.h>
#include <stdlib.h>


#define ARRAY_INI 456;

TokenList* tl_init() {
	TokenList* tl = arena_alloc(main_arena, sizeof(TokenList));
	if (!tl) {
		return NULL;
	}
	tl->cap = ARRAY_INI;
	tl->count = 0;
	tl->items = arena_alloc(main_arena, sizeof(Token)*tl->cap);
	return tl;
}

void tl_add(TokenList *tl, Token t) {
	if (tl->count >= tl->cap) {
		size_t new_size = tl->cap*2;
		tl->items = arena_realloc(main_arena, tl->items, tl->cap, new_size);
		if (!tl->items) return;
	}
	tl->items[tl->count++] = t;
}

Token tl_get(TokenList *tl, size_t n) {
	if (n >= tl->count) {
		// the last token in the array is always EOS
		return tl_get(tl, tl->count-1);
	}
	return tl->items[n];
}

void tl_free(TokenList* tl) {	
	// inner data will be freed along with the arena
	free(tl);
}

Span create_span(const char *filename, size_t line, size_t column, size_t colend) {
	return (Span) {arena_strdup(main_arena, filename), line, column, colend};
}

Token create_token(const char *lexme, Span span) {
	return (Token) {arena_strdup(main_arena, lexme), span};
}
