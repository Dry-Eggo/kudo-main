#pragma once

#include "type_system.h"
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct TypeMap TypeMap;
    typedef struct VarMap VarMap;
    typedef struct SourceManager SourceManager;

    TypeMap* tm_create();
    void     tm_free(TypeMap* tm);
    void     tm_put(TypeMap* tm, const char* k, Type* v);
    bool     tm_has(TypeMap* tm, const char* k);
    Type*    tm_get(TypeMap* tm, const char* k);

    VarMap* vm_create();
    void    vm_free(VarMap* tm);

    SourceManager* sm_create(const char* source);
    const char* sm_get(SourceManager*, size_t n);
    void sm_free(SourceManager* sm);
#ifdef __cplusplus
}
#endif
