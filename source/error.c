#include <stdlib.h>
#include <error.h>
#include <string.h>

Error* create_error_typemismatch(const char* expected, const char* got, Span s) {
    Error* e = NEW(Error);
    e->kind = ERROR_TYPEMISMATCH;
    e->e_tm.span = s;
    e->e_tm.expected = str_dup(expected);
    e->e_tm.got = str_dup(got);
    return e;
}

Error* create_error_invalidtype(Span s) {
    Error* e = NEW(Error);
    e->kind = ERROR_INVALIDTYPE;
    e->e_it.span = s;
    return e;
}

