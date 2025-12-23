#include "ast.h"


ASTNode* ast_new_literal(Token* t)
{ 
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode)); 
    n->type = AST_LITERAL; 
    n->as.literal.value = my_strdup(t->lexeme); 
    n->location = t->location;  
    return n; 
}

ASTNode* ast_new_identifier(Token* t) { 
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode)); 
    n->type = AST_IDENTIFIER; 
    n->as.ident.name = my_strdup(t->lexeme); 
    n->location = t->location;
    return n; 
}

ASTNode* ast_new_unary(Token* op, ASTNode* operand) 
{ 
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode)); 
    n->type = AST_UNARY; 
    n->as.unary.op = op; 
    n->as.unary.operand = operand; 
    n->location = op->location;
    return n; 
}

ASTNode* ast_new_binary(ASTNode* left, Token* op, ASTNode* right) 
{ 
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode)); 
    n->type = AST_BINARY; 
    n->as.binary.left = left; 
    n->as.binary.op = op; 
    n->as.binary.right = right; 
    n->location = op->location;
    return n; 
}

ASTNode* ast_new_assign(Token* name, Token* op, ASTNode* value)
{ 
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode)); 
    n->type = AST_ASSIGN; 
    n->as.assign.name = name; 
    n->as.assign.op = op; 
    n->as.assign.value = value; 
    n->location = name->location;
    return n; 
}

ASTNode* ast_new_index(ASTNode* base, ASTNode* index) 
{ 
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode)); 
    n->type = AST_INDEX; 
    n->as.idx.base = base; 
    n->as.idx.index = index; 
    n->location = base->location; // Use base's location
    return n; 
}

ASTNode* ast_new_call(ASTNode* callee, ASTNode** args, int arg_count) 
{ 
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode)); 
    n->type = AST_FN_CALL; 
    n->as.call.callee = callee; 
    n->as.call.args = args; 
    n->as.call.arg_count = arg_count; 
    n->location = callee->location; // Use callee's location
    return n; 
}

ASTNode* ast_new_range(ASTNode* start, ASTNode* end, ASTNode* inc_end, ASTNode* step) 
{ 
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode)); 
    n->type = AST_RANGE; 
    n->as.rng.start = start; 
    n->as.rng.end = end; 
    n->location = start->location;
    return n; 
}


ASTNode* ast_for_expr(ASTNode* variable, ASTNode* expr)
{
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode));
    n->type = AST_FOR_EXPR;
    n->as.forexpr.variable = variable;
    n->as.forexpr.expr = expr;
    n->location = variable->location;
    return n;
}