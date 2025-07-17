#pragma once

#include <arena.h>

#define elog(fmt, ...)    fprintf(stderr, "\033[33mKudo: " fmt "\033[0m", ##__VA_ARGS__)
#define NEW(type)         (type*)arena_alloc(main_arena, sizeof(type))
#define NEWSZ(type, size) (type*)arena_alloc(main_arena, sizeof(type) * (size))
#define str_dup(cstr)     arena_strdup(main_arena, cstr)


extern Arena *main_arena;
