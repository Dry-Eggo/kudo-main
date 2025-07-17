#include <nodes.h>
#include <globals.h>

Expr* create_expr_int(int value, Span s) {
    Expr* e = NEW(Expr);
    e->kind = EXPR_INTEGER;
    e->int_expr = value;
    e->span = s;
    return e;
}

Expr* create_expr_string(const char* value, Span s) {
    Expr* e = NEW(Expr);
    e->kind = EXPR_STRING;
    e->string_expr = str_dup(value);
    e->span = s;
    return e;
}

Expr* create_expr_cstring(const char* value, Span s) {
    Expr* e = NEW(Expr);
    e->kind = EXPR_CSTRING;
    e->string_expr = str_dup(value);
    e->span = s;
    return e;
}

Stmt* create_stmt_vardeclauto(const char* name, Expr* expr, Span s) {
    Stmt* stmt = NEW(Stmt);
    stmt->kind = STMT_VARDECLAUTO;
    stmt->var_decl_auto.identifier = name;
    stmt->var_decl_auto.value = expr;
    stmt->span = s;
    return stmt;
}

Stmt* create_stmt_vardecl(const char* name, Expr* expr, Type* type, Span s) {
    Stmt* stmt = NEW(Stmt);
    stmt->kind = STMT_VARDECL;
    stmt->span = s;
    stmt->var_decl.identifier = name;
    stmt->var_decl.value = expr;
    stmt->var_decl.type  = type;
    return stmt;
}

Expr* create_stmt_compound(list_Stmt* stmts) {
    Expr* e = NEW(Expr);
    e->kind = EXPR_COMPOUND_STMT;
    e->compound_expr = NEW(CompoundStmt);
    e->compound_expr->statements = stmts;
    return e;
}

Stmt* create_stmt_funcdecl(const char* name, list_Expr* params, Type* ret, Expr* body, Span s) {
    Stmt* stmt = NEW(Stmt);
    stmt->kind = STMT_FUNCDECL;
    stmt->span = s;
    stmt->func_decl.name = name;
    stmt->func_decl.params = params;
    stmt->func_decl.return_type = ret;
    stmt->func_decl.body = body;
    return stmt;
}
