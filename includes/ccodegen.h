#pragma once

#include <nodes.h>
#include <nob.h>
#include <string_builder.h>
#include <options.h>

typedef struct CCodegenState {    
    list_Stmt*            program;
    Nob_String_Builder    source;
    StringBuilder*        output;
    Args*                 options;
} CCodegenState;

CCodegenState* kudo_ccodegen_init(list_Stmt* p, Args* args, Nob_String_Builder source);
void kudo_ccodegen_kickoff(CCodegenState*);
