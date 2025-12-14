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
