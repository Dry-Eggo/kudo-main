#pragma once

#include <memory>
#include <variant>
#include <vector>

#define uniq(type) std::unique_ptr<type>
#define shr(type)  std::shared_ptr<type>
#define mk_u(type, ...) std::make_unique<type>(__VA_ARGS__)
#define mk_s(type, ...) std::make_shared<type>(__VA_ARGS__)
#define mv(value) std::move(value)
#define as(type, v) std::get<type>((v)->data).get()

template<typename T, typename VariantHolder>
bool is(const VariantHolder& v) {
    return std::holds_alternative<T>(v);
}

namespace Kudo::Language {
struct Token;
struct Expr;
struct Stmt;

struct Integer;
struct IdentN;
struct StringN;
struct CStringN;
struct FunctionCall;
struct Body;
struct ExprStmt;
struct VariableDecl;
struct FunctionDef;
struct ExternDef;
struct BinaryOp;
struct UnaryOp;

typedef std::vector<Token> Tokens;
using ExprPtr = std::unique_ptr<Expr>;
using StmtPtr = std::unique_ptr<Stmt>;
using ExprVariant = std::variant<uniq(Integer), uniq(StringN), uniq(Body), uniq(CStringN), uniq(FunctionCall), uniq(IdentN), uniq(BinaryOp), uniq(UnaryOp)>;
using StmtVariant =
    std::variant<uniq(FunctionDef), uniq(VariableDecl), uniq(ExprStmt), uniq(ExternDef)>;
} // namespace Kudo::Language
