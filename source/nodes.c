#include <nodes.h>
#include <globals.h>

Expr* create_expr_int(int value) {
    Expr* e = NEW(Expr);
    e->kind = EXPR_INTEGER;
    e->int_expr = value;
    return e;
}

Expr* create_expr_string(const char* value) {
    Expr* e = NEW(Expr);
    e->kind = EXPR_STRING;
    e->string_expr = str_dup(value);
    return e;
}

Stmt* create_stmt_vardeclauto(const char* name, Expr* expr) {
    Stmt* stmt = NEW(Stmt);
    stmt->kind = STMT_VARDECLAUTO;
    stmt->var_decl_auto.identifier = name;
    stmt->var_decl_auto.value = expr;
    return stmt;
}

Expr* create_stmt_compound(list_Stmt* stmts) {
    Expr* e = NEW(Expr);
    e->kind = EXPR_COMPOUND_STMT;
    e->compound_expr = NEW(CompoundStmt);
    e->compound_expr->statements = stmts;
    return e;
}

Stmt* create_stmt_funcdecl(const char* name, list_Expr* params, Type* ret, Expr* body) {
    Stmt* stmt = NEW(Stmt);
    stmt->kind = STMT_FUNCDECL;
    stmt->func_decl.name = name;
    stmt->func_decl.params = params;
    stmt->func_decl.return_type = ret;
    stmt->func_decl.body = body;
    return stmt;
}
