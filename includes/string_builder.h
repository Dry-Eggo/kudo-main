#pragma once
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arena.h"
#include "globals.h"

#define SB_INI 255
#define sbtcstr(sb) (sb)->data
#define sbapp(sb, ...) SB_append(sb, __VA_ARGS__);
#define sbeq(sb, cstr) (strcmp(sbtcstr(sb), cstr) == 0)

typedef struct {
  char *data;
  int len;
  int cap;
} StringBuilder;

static void SB_init(StringBuilder **sb) {
  *sb = (StringBuilder *)arena_alloc(main_arena, sizeof(StringBuilder));
  (*sb)->data = (char *)arena_alloc(main_arena, SB_INI);
  (*sb)->len = 0;
  (*sb)->cap = SB_INI;
  (*sb)->data[0] = '\0';
}

static void SB_append(StringBuilder *sb, const char *cstr, ...) {
  va_list args;
  va_start(args, cstr);
  int needed = vsnprintf(NULL, 0, cstr, args);
  va_end(args);
  int tneeded = sb->len + needed;
  if (tneeded + 1 > sb->cap) {
	  size_t old_size = sb->cap;
    while (sb->cap < tneeded + 1) {
      sb->cap *= 2;
    }
    char *newbuf = (char *)arena_realloc(main_arena, sb->data, old_size, sb->cap);
    sb->data = newbuf;
  }
  va_start(args, cstr);
  int written = vsnprintf(sb->data + sb->len, sb->cap - sb->len, cstr, args);
  sb->len += written;
  sb->data[sb->len] = '\0';
  va_end(args);
}
static void SB_set(StringBuilder *sb, const char *cstr, ...) {
  va_list args;
  va_start(args, cstr);
  int needed = vsnprintf(NULL, 0, cstr, args);
  va_end(args);
  int tneeded = sb->len + needed;
  if (tneeded + 1 > sb->cap) {
	  size_t old_size = sb->cap;
    while (sb->cap < tneeded + 1) {
      sb->cap *= 2;
    }
    char *newbuf = (char *)arena_realloc(main_arena, sb->data, old_size, sb->cap);
    sb->data = newbuf;
  }
  va_start(args, cstr);
  sb->len = needed;
  sb->data[sb->len] = '\0';
  va_end(args);
}
static void SB_free(StringBuilder *s) {
  s = NULL;
}
