#include "ast.h"
// this file contains implementations for declarations

ASTNode* ast_var_decl(ASTNode* ident, Token* data_type, ASTNode* value)
{
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode));
    n->type = AST_VAR_DECL;
    n->as.declaration.data_type = data_type;
    n->as.declaration.ident = ident;
    n->as.declaration.value = value;
    n->location = ident->location; // debug print line of the identifier of a var declaration
    return n;
}

ASTNode* ast_const_decl(ASTNode* ident, Token* data_type, ASTNode* value)
{
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode));
    n->type = AST_CONST_DECL;
    n->as.declaration.data_type = data_type;
    n->as.declaration.ident = ident;
    n->as.declaration.value = value;
    n->location = ident->location;
    return n;
}

ASTNode* ast_new_array(ASTNode* ident, Token* type, ASTNode* range, ASTNode** literals, size_t literal_count)
{
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode));
    n->type = AST_ARRAY_DECL;
    n->as.arr.ident = ident;
    n->as.arr.type = type;
    n->as.arr.range = range;
    n->as.arr.literals = literals;
    n->as.arr.literal_count = literal_count;
    n->location = ident->location;
    return n;
}

ASTNode* ast_param(ASTNode* ident, Token* type)
{
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode));
    n->type = AST_PARAM;
    n->as.param.ident = ident;
    n->as.param.type = type;
    n->location = ident->location;
    return n;
}

ASTNode* ast_fn_decl(ASTNode* ident, 
                     ASTNode** params, 
                     size_t params_count, 
                     Token* return_type, 
                     ASTNode* block)
{
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode));
    n->type = AST_FN_DECL;
    n->as.func.ident = ident;
    n->as.func.params = params;
    n->as.func.params_count = params_count;
    n->as.func.return_type = return_type;
    n->as.func.block = block;
    n->location = ident->location;
    return n;
}

ASTNode* ast_return_stmt(ASTNode* expr)
{
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode));
    n->type = AST_RETURN;
    n->as.return_stmt.expr = expr;
    n->location = expr->location;
    return n;
}