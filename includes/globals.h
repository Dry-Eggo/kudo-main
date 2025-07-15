#pragma once

#include <arena.h>

#define elog(fmt, ...) fprintf(stderr, "\033[33mKudo: "fmt"\033[0m", ##__VA_ARGS__)

extern Arena *main_arena;
