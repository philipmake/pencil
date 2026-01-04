#include "ast.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// helper function. strdup is POSIX not standard C
char* my_strdup(const char* s) 
{
    size_t len = strlen(s) + 1;
    char* copy = malloc(len);
    if (copy) memcpy(copy, s, len);
    return copy;
}

void astnodetype_to_string(ASTNodeType type)
{
    switch (type){
        case AST_BLOCK:
            printf("AST_BLOCK\n");
            break;
        case AST_PROGRAM:
            printf("AST_PROGRAM\n");
            break;
        case AST_STMT:
            printf("AST_STMT\n");
            break;
        case AST_LITERAL:
            printf("AST_LITERAL\n");
            break;
        case AST_IDENTIFIER:
            printf("AST_IDENTIFIER\n");
            break;
        case AST_UNARY:
            printf("AST_UNARY\n");
            break;
        case AST_BINARY:
            printf("AST_BINARY\n");
            break;
        case AST_ASSIGN:
            printf("AST_ASSIGN\n");
            break;
        case AST_INDEX:
            printf("AST_INDEX\n");
            break;
        case AST_FN_CALL:
            printf("AST_FN_CALL\n");
            break;
        case AST_RANGE:
            printf("AST_RANGE\n");
            break;
        case AST_VAR_DECL:
            printf("AST_VAR_DECL\n");
            break;
        case AST_CONST_DECL:
            printf("AST_CONST_DECL\n");
            break;
        case AST_FN_DECL:
            printf("AST_FN_DECL\n");
            break;
        case AST_ARRAY_DECL:
            printf("AST_ARRAY_DECL\n");
            break;
        case AST_PARAM:
            printf("AST_PARAM\n");
            break;
        case AST_STRUCT:
            printf("AST_STRUCT\n");
            break;
        case AST_ENUM:
            printf("AST_ENUM\n");
            break;
        case AST_VECTOR:
            printf("AST_VECTOR\n");
            break;
        case AST_FIELD:
            printf("AST_FIELD\n");
            break;
        case AST_UNION:
            printf("AST_UNION\n");
            break;
        case AST_IF:
            printf("AST_IF\n");
            break;
        case AST_MATCH:
            printf("AST_MATCH\n");
            break;
        case AST_MATCH_CASE:
            printf("AST_MATCH_CASE\n");
            break;
        case AST_LOOP:
            printf("AST_LOOP\n");
            break;
        case AST_LOOP_EXPR:
            printf("AST_LOOP_EXPR\n");
            break;
        case AST_RETURN:
            printf("AST_RETURN\n");
            break;
        default:
            printf("Default cae: UNKOWN ASTNODE\n");
            break;
    }
   
}

// ===== Allocator =====
void* parser_alloc(size_t size) 
{
    void* m = malloc(size);
    if (!m) { fprintf(stderr, "Out of memory\n"); exit(1);}
    memset(m, 0, size);
    return m;
}


ASTNode* ast_program(void)
{
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode));
    n->type = AST_PROGRAM;

    n->as.program.statements = NULL;
    n->as.program.stmt_count = 0;
    
    return n;
}

void add_stmt(ASTNode* program, ASTNode* stmt) 
{
    program->as.program.statements = realloc(
        program->as.program.statements,
        sizeof(ASTNode*) * (program->as.program.stmt_count + 1)
    );
    program->as.program.statements[program->as.program.stmt_count++] = stmt;
}

ASTNode* ast_block(ASTNode** statements, size_t count)
{
    ASTNode* n = malloc(sizeof(ASTNode));
    n->type = AST_BLOCK;
    n->as.block.statements = statements;
    n->as.block.count = count;
    // n->location = statements->location;
    return n;
}
