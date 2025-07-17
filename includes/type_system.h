#pragma once
#include <enum.h>

typedef struct Type {
    BaseType type;
    const char* final_repr;
} Type;

Type* create_type(BaseType type);
Type* builtin_create_type(BaseType type, const char* fr);
