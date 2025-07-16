#include <ccodegen.h>
#include <globals.h>

typedef struct {
    StringBuilder* result;
    StringBuilder* preamble;
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
void cerror(const char* msg);

const char* type_tostr(Type* type);


[[noreturn]]void cerror(const char* msg) {
    elog("Codegen Failed: %s\n", msg);
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
	sbapp(sb, "\nint %s = %s;", v.identifier, res.result->data);
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
    sbapp(sb, "\n%s %s() {", type_tostr(func.return_type), func.name);
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
	} break;
    default:
	cerror("Invalid Expr");
    }
    return res;
}

const char* type_tostr(Type* type) {
    if (type->type == TYPE_VOID) {
	return "void";
    } else if (type->type == TYPE_INT) {
	return "int";
    }
    cerror("Unknown type");
}

CCodegenState* kudo_ccodegen_init(list_Stmt* p, Args* args, Nob_String_Builder source) {
    CCodegenState* cs = NEW(CCodegenState);
    cs->program = p;
    cs->options = args;
    cs->source  = source;
    SB_init(&cs->output);
    return cs;
}

void kudo_ccodegen_kickoff(CCodegenState* cs) {
    int max = cs->program->count;
    for (int i = 0; i < max; ++i) {
	Stmt* stmt = list_Stmt_get(cs->program, i);
	gen_stmt(cs, stmt);
    }
}
