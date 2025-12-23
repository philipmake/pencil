#include "ast.h"

ASTNode* ast_if(ASTNode* condition,
                ASTNode* then_branch,
                ASTNode* else_branch) // can be NULL
{
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode));
    n->type = AST_IF;
    n->as.ifstmt.condition = condition;
    n->as.ifstmt.then_branch = then_branch;
    n->as.ifstmt.else_branch = else_branch;

    n->location = condition->location;
    return n;
}


ASTNode* ast_new_match_case(ASTNode* expr, 
                            ASTNode* stmt)
{
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode));
    n->type = AST_MATCH_CASE;
    n->as.matchcase.expr = expr;
    n->as.matchcase.stmt = stmt;
    n->location = expr->location;
    return n;
}


ASTNode* ast_new_match_stmt(ASTNode* pattern, 
                            ASTNode** match_cases, 
                            size_t case_count, 
                            ASTNode* def_case)
{
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode));
    n->type =  AST_MATCH;
    n->as.matchstmt.pattern = pattern;
    n->as.matchstmt.match_cases = match_cases;
    n->as.matchstmt.case_count = case_count;
    n->as.matchstmt.def_case = def_case;
    n->location = pattern->location;
    return n;
}

ASTNode* ast_for_loop(ASTNode* expr, ASTNode* block)
{
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode));
    n->type =  AST_FOR;
    n->as.forloop.expr =  expr;
    n->as.forloop.block = block;
    n->location = expr->location;
    return n;
}

ASTNode* ast_loop(ASTNode* block)
{
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode));
    n->type =  AST_LOOP;
    n->as.loop.block = block;
    n->location = block->location;
    return n;
}

