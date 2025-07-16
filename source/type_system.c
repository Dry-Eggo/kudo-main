#include <type_system.h>
#include <globals.h>

Type* create_type(BaseType type) {
    Type* t = NEW(Type);
    t->type = type;
    return t;
}
