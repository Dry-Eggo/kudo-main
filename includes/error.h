#pragma once

#include "token.h"
#include <globals.h>
#include <list_builder.h>
#include <arena.h>

typedef struct ErrorTypeMisatch {
    const char* expected;
    const char* got;
    Span  span;
} ErrorTypeMisatch;

typedef struct ErrorInvalidType {
    Span span;
} ErrorInvalidType;

typedef struct {
    ErrorKind kind;
    union {
        struct ErrorInvalidType e_it;
        struct ErrorTypeMisatch e_tm;
    };
} Error;

NEW_LIST(Error)

Error* create_error_typemismatch(const char* expected, const char* got, Span s);
Error* create_error_invalidtype(Span s);
