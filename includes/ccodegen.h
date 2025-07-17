#pragma once

#include "error.h"
#include <nodes.h>
#include <nob.h>
#include <string_builder.h>
#include <options.h>
#include <khm.h>

typedef struct CCodegenState {    
    list_Stmt*            program;
    list_Error*           errors;
    Nob_String_Builder    source;
    StringBuilder*        output;
    SourceManager*        source_manager;
    Args*                 options;

    TypeMap*              type_map;
    VarMap*               var_map;
} CCodegenState;

CCodegenState* kudo_ccodegen_init(list_Stmt* p, Args* args, Nob_String_Builder source);
void kudo_ccodegen_kickoff(CCodegenState*);
