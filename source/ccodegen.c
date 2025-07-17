#include <ccodegen.h>
#include <globals.h>
#include <stdlib.h>


static const char* HEADER_PATH = NULL;

typedef struct {
    StringBuilder* result;
    StringBuilder* preamble;
    Type*          type;
} CResult;

void new_result(CResult* cr) {
    SB_init(&cr->result); SB_init(&cr->preamble);
}

CResult noresult() {
    return (CResult) {};
}

CResult create_res(StringBuilder* res, StringBuilder* pre) {
    return (CResult) { res, pre };
}

CResult gen_expr(CCodegenState* cs, Expr* expr);
CResult gen_stmt(CCodegenState* cs, Stmt* stmt);
CResult gen_body(CCodegenState* cs, Expr* body);

void gen_function(CCodegenState* cs, FuncDecl func);
void cerror(CCodegenState* cs, Error* error);

const char* type_torepr(Type* type);
const char* type_tostr(Type* type);
bool type_match(Type* t1, Type* t2);


[[noreturn]]void cerror(CCodegenState* cs, Error* error) {
    if (error->kind == ERROR_INVALIDTYPE) {
	Span span = error->e_it.span;
	const char* line = sm_get(cs->source_manager, span.line-1);
	fprintf(stdout, "Kudo Error: %s:%ld:%ld:  Invalid Type\n", span.filename, span.line, span.column);
    }
    else if (error->kind == ERROR_TYPEMISMATCH) {
	Span span = error->e_tm.span;
	const char* line = sm_get(cs->source_manager, span.line-1);
	fprintf(stderr, "Kudo Error: %s:%ld:%ld: Type Mismatch. Expected '%s' got '%s' instead.\n", span.filename, span.line, span.column, error->e_tm.expected, error->e_tm.got);
	fprintf(stderr, " %1c |\n", ' ');
	fprintf(stderr, " %1ld | %s\n", span.line, line);
	fprintf(stderr, " %1c |", ' ');
    }
    exit(1);
}

CResult gen_stmt(CCodegenState* cs, Stmt* stmt) {
    if (stmt->kind == STMT_FUNCDECL) {
        gen_function(cs, stmt->func_decl);
        return noresult();
    }
    else if (stmt->kind == STMT_VARDECLAUTO) {
        VarDeclAuto v = stmt->var_decl_auto;
        StringBuilder* sb; SB_init(&sb);
        CResult res = gen_expr(cs, v.value);	
        sbapp(sb, "\n%s %s = %s;", type_torepr(res.type), v.identifier, res.result->data);
        return create_res(sb, NULL);
    } else if (stmt->kind == STMT_VARDECL) {
        VarDecl v = stmt->var_decl;
        StringBuilder* sb; SB_init(&sb);
        if (!tm_has(cs->type_map, type_tostr(v.type))) {
            elog("No Such Type: '%s'", type_tostr(v.type));
            exit(1);
        }
        CResult res = gen_expr(cs, v.value);	
        if (!type_match(res.type, v.type)) {
	    printf("Here\n");
            cerror(cs, create_error_typemismatch(type_tostr(v.type), type_tostr(res.type), stmt->span));
        }
        sbapp(sb, "\n%s %s = %s;", type_torepr(res.type), v.identifier, res.result->data);
        return create_res(sb, NULL);
    }
    return noresult();
}

CResult gen_body(CCodegenState* cs, Expr* body) {
    CResult res; new_result(&res);
    if (body->kind == EXPR_COMPOUND_STMT) {
        CompoundStmt* cstmt = body->compound_expr;
        if (cstmt->statements) {
            int max = cstmt->statements->count;
            for (int i = 0; i < max; ++i) {
                Stmt* stmt = list_Stmt_get(cstmt->statements, i);
                sbapp(res.result, "%s", gen_stmt(cs, stmt).result->data);
            }
        }
    }
    return res;
}

void gen_function(CCodegenState* cs, FuncDecl func) {
    StringBuilder* sb;
    SB_init(&sb);
    sbapp(sb, "\n%s %s() {", type_torepr(func.return_type), func.name);
    CResult body = gen_body(cs, func.body);
    sbapp(sb, "%s", body.result->data);
    sbapp(sb, "\n}\n");
    sbapp(cs->output, sb->data);
}


CResult gen_expr(CCodegenState* cs, Expr* expr) {
    CResult res; new_result(&res);

    switch (expr->kind)
    {
        case EXPR_INTEGER: {
            sbapp(res.result, "%d", expr->int_expr);
            res.type = create_type(TYPE_INT);
        } break;
        case EXPR_CSTRING: {
            sbapp(res.result, "\"%s\"", expr->string_expr);
            res.type = create_type(TYPE_CSTR);
        } break;
        default:
            cerror(cs, create_error_invalidtype(expr->span));
    }
    return res;
}

const char* type_tostr(Type* type) {
    if (type->type == TYPE_VOID) {
        return "void";
    } else if (type->type == TYPE_INT) {
        return "int";
    } else if (type->type == TYPE_CSTR) {
        return "cstr";
    }
    elog("Invalid Type\n");
    exit(1);
}
const char* type_torepr(Type* type) {
    if (type->type == TYPE_VOID) {
        return "void";
    } else if (type->type == TYPE_INT) {
        return "kudo_int";
    } else if (type->type == TYPE_CSTR) {
        return "char*";
    }
    elog("Invalid Type\n");
    exit(1);
}

CCodegenState* kudo_ccodegen_init(list_Stmt* p, Args* args, Nob_String_Builder source) {
    CCodegenState* cs = NEW(CCodegenState);
    cs->program = p;
    cs->options = args;
    cs->source  = source;
    cs->type_map = tm_create();
    cs->source_manager  = sm_create(source.items);
    // cs->var_map  = vm_create();
    SB_init(&cs->output);

    HEADER_PATH = getenv("KUDO_HEADER");
    if (!HEADER_PATH) {
        elog("KUDO_HEADER environment not set.\n");
        exit(1);
    }
    sbapp(cs->output, "#include \"%s\"\n", HEADER_PATH);

    // Initialize builtin types
    Type* kudo_int_type = builtin_create_type(TYPE_INT, "kudo_int");
    Type* kudo_cstr_type = builtin_create_type(TYPE_CSTR, "char*");
    tm_put(cs->type_map, "int", kudo_int_type);
    tm_put(cs->type_map, "cstr", kudo_cstr_type);
    return cs;
}

void kudo_ccodegen_kickoff(CCodegenState* cs) {
    int max = cs->program->count;
    for (int i = 0; i < max; ++i) {
        Stmt* stmt = list_Stmt_get(cs->program, i);
        gen_stmt(cs, stmt);
    }

    sm_free(cs->source_manager);
    tm_free(cs->type_map);
}


bool type_match(Type* t1, Type* t2) {
    if (t1->type == t2->type) return true;
    return false;
}   
