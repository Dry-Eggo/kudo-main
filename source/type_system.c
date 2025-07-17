#include <type_system.h>
#include <globals.h>

Type* create_type(BaseType type) {
    Type* t = NEW(Type);
    t->type = type;
    t->final_repr = NULL;
    return t;
}

Type* builtin_create_type(BaseType type, const char* fr) {
    Type* t = NEW(Type);
    t->type = type;
    t->final_repr = arena_strdup(main_arena, fr);
    return t;
}

