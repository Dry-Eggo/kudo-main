#pragma once
#include <enum.h>

typedef struct Type {
    BaseType type;
} Type;

Type* create_type(BaseType type);
