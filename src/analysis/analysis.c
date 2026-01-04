#include "analysis.h"
#include "ast.h"
#include "parser.h"
#include "token.h"
#include <stdio.h>


// analysis starts by taking a program node.
void start_analysis(ASTNode* prog)
{
    if (prog->type != AST_PROGRAM) return;

    ("Starting analysis...\n");

    for (int i = 0; i < prog->as.program.stmt_count; i++)
    {
        ASTNode* node = prog->as.program.statements[i];
        ASTNodeType type = node->type;
        astnodetype_to_string(type);
        analyse_node(node);
    }

    ("End of analysis\n");
}


// check for nodes in a node
void analyse_node(ASTNode* node)
{
    ASTNodeType type = node->type;

    switch (type){
        case AST_BLOCK:
        {
            break;
        }
        case AST_LITERAL:
            break;
        case AST_IDENTIFIER:
            break;
        case AST_UNARY:
            break;
        case AST_BINARY:
            break;
        case AST_ASSIGN:
            break;
        case AST_INDEX:
            break;
        case AST_FN_CALL:
            break;
        case AST_RANGE:
            break;
        case AST_VAR_DECL:
        case AST_CONST_DECL:
        {
            Token* type = node->as.declaration.data_type;
            ASTNode* identifier = node->as.declaration.ident;
            ASTNode* value = node->as.declaration.value;
            // TODO: Check that type is set else infer the type from 
            // the literal or expression assigned to the variable or constant
            printf("   Declaration analysed\n");
            break;
        }
        case AST_FN_DECL:
        {
            // params
            if (node->as.func.params_count != 0) 
            {
                for (int i = 0; i < node->as.func.params_count; i++)
                {
                    if (node->as.func.params[i] != NULL)
                        analyse_node(node->as.func.params[i]);
                }
            }
             
            // return type
            // block 
            ASTNode* block = node->as.func.block;
            int blk_stmt_count = block->as.block.count;
            for (int i = 0; i < blk_stmt_count; i++)
            {
                ASTNodeType block_node_type = block->as.block.statements[i]->type;
                printf(" - ");
                astnodetype_to_string(block_node_type);
                analyse_node(block->as.block.statements[i]);
            }
            break;
        }
        case AST_ARRAY_DECL:
            break;
        case AST_PARAM:
        {
            astnodetype_to_string(node->type);
            break;
        }
        case AST_STRUCT:
            break;
        case AST_ENUM:
            break;
        case AST_VECTOR:
            break;
        case AST_FIELD:
            break;
        case AST_UNION:
            break;
        case AST_IF:
            break;
        case AST_MATCH:
            break;
        case AST_MATCH_CASE:
            break;
        case AST_LOOP:
            break;
        case AST_LOOP_EXPR:
            break;
        case AST_RETURN:
            break;
        default:
            break;
    }   
}
