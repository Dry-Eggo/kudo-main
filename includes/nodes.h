#pragma once

#include "token.h"
#include <enum.h>
#include <list_builder.h>
#include <type_system.h>
#include <stdlib.h>
#include <globals.h>

typedef struct Expr Expr;
typedef struct Stmt Stmt;
NEW_LIST(Expr)
NEW_LIST(Stmt)

typedef struct CompoundStmt {
    list_Stmt*  statements;
} CompoundStmt;

typedef struct VarDeclAuto {
    const char* identifier;
    Expr*       value;
} VarDeclAuto;

typedef struct VarDecl {
    Type*        type;
    const char*  identifier;
    Expr*        value;
} VarDecl;

typedef struct FuncDecl {
    const char* name;
    list_Expr*  params;
    Expr*       body;
    Type*       return_type;
} FuncDecl;

struct Expr {
    ExprKind kind;
    Span span;
    union {
        int             int_expr;
        const char*     string_expr;
        struct CompoundStmt* compound_expr;
    };
};

struct Stmt {
    StmtKind kind;
    Span span;
    union {
        struct FuncDecl func_decl;
        struct VarDeclAuto var_decl_auto;
        struct VarDecl     var_decl;
        struct Expr*       expr_stmt;
    };
};

Expr* create_expr_int(int value, Span s);
Expr* create_expr_string(const char* value, Span s);
Expr* create_expr_cstring(const char* value, Span s);
Expr* create_stmt_compound(list_Stmt* stmts);
Stmt* create_stmt_vardeclauto(const char* name, Expr* expr, Span s);
Stmt* create_stmt_vardecl(const char* name, Expr* expr, Type* type, Span s);
Stmt* create_stmt_funcdecl(const char* name, list_Expr* params, Type* ret, Expr* body, Span s);
