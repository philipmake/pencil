#include <stdio.h>
#include "ast.h"

void print_indent(int indent) 
{
    for (int i = 0; i < indent; i++)
        printf("  ");
}

void print_location(SourceLocation location)
{
    if (location.filename) {
        printf(" [%s:%d:%d]", location.filename, location.line, location.column);
    } else {
        printf(" [line:%d col:%d]", location.line, location.column);
    }
}

void print_ast(ASTNode* node, int indent);

void print_statements(ASTNode** stmts, size_t count, int indent) 
{
    if (!stmts) return;
    for (size_t i = 0; i < count; i++)
        print_ast(stmts[i], indent);
    printf("\n");
}

void print_ast(ASTNode* node, int indent) 
{
    if (!node) {
        print_indent(indent);
        printf("(null)\n");
        return;
    }

    print_indent(indent);

    switch (node->type) 
    {
        // =================== LITERALS & IDENTIFIERS ===================
        case AST_IDENTIFIER:
            printf("Identifier: %s", node->as.ident.name);
            print_location(node->location);
            printf("\n");
            break;

        case AST_LITERAL:
            printf("Literal: %s", node->as.literal.value);
            print_location(node->location);
            printf("\n");
            break;

        // =================== EXPRESSIONS ===================
        case AST_UNARY:
            printf("Unary(%s)", node->as.unary.op->lexeme);
            print_location(node->location);
            printf("\n");
            print_ast(node->as.unary.operand, indent + 1);
            break;

        case AST_BINARY:
            printf("Binary(%s)", node->as.binary.op->lexeme);
            print_location(node->location);
            printf("\n");
            print_ast(node->as.binary.left, indent + 1);
            print_ast(node->as.binary.right, indent + 1);
            break;

        case AST_ASSIGN:
            printf("Assign(%s %s ...)", 
                   node->as.assign.name->lexeme,
                   node->as.assign.op->lexeme);
            print_location(node->location);
            printf("\n");
            print_ast(node->as.assign.value, indent + 1);
            break;

        case AST_INDEX:
            printf("Index");
            print_location(node->location);
            printf("\n");
            print_indent(indent + 1);
            printf("Base:\n");
            print_ast(node->as.idx.base, indent + 2);
            print_indent(indent + 1);
            printf("Index:\n");
            print_ast(node->as.idx.index, indent + 2);
            break;

        case AST_FN_CALL:
            printf("FnCall");
            print_location(node->location);
            printf("\n");
            print_indent(indent + 1);
            printf("Callee:\n");
            print_ast(node->as.call.callee, indent + 2);
            print_indent(indent + 1);
            printf("Args (%d):\n", node->as.call.arg_count);
            for (int i = 0; i < node->as.call.arg_count; i++) {
                print_ast(node->as.call.args[i], indent + 2);
            }
            break;

        case AST_RANGE:
            printf("Range");
            print_location(node->location);
            printf("\n");
            print_indent(indent + 1);
            printf("Start:\n");
            print_ast(node->as.rng.start, indent + 2);
            print_indent(indent + 1);
            printf("End:\n");
            print_ast(node->as.rng.end, indent + 2);
            break;

        // =================== DECLARATIONS ===================
        case AST_VAR_DECL:
            printf("VarDecl");
            print_location(node->location);
            printf("\n");
            print_ast(node->as.declaration.ident, indent + 1);
            print_indent(indent + 1);
            printf("Type: %s\n", 
                   node->as.declaration.data_type ? node->as.declaration.data_type->lexeme : "inferred");
            if (node->as.declaration.value) {
                print_indent(indent + 1);
                printf("Value:\n");
                print_ast(node->as.declaration.value, indent + 2);
            }
            break;

        case AST_CONST_DECL:
            printf("ConstDecl(%s: %s)",
                   node->as.declaration.ident->as.ident.name,
                   node->as.declaration.data_type ? node->as.declaration.data_type->lexeme : "inferred");
            print_location(node->location);
            printf("\n");
            if (node->as.declaration.value) {
                print_indent(indent + 1);
                printf("Value:\n");
                print_ast(node->as.declaration.value, indent + 2);
            }
            break;

        case AST_ARRAY_DECL:
            printf("ArrayDecl");
            print_location(node->location);
            printf("\n");
            print_indent(indent + 1);
            printf("Identifier: %s\n", node->as.arr.ident->as.ident.name);
            print_indent(indent + 1);
            printf("Type: %s\n", 
                   node->as.arr.type ? node->as.arr.type->lexeme : "unknown");
            
            if (node->as.arr.range) {
                print_indent(indent + 1);
                printf("Range:\n");
                print_ast(node->as.arr.range, indent + 2);
            }
            
            if (node->as.arr.literals && node->as.arr.literal_count > 0) {
                print_indent(indent + 1);
                printf("Literals (%zu):\n", node->as.arr.literal_count);
                for (size_t i = 0; i < node->as.arr.literal_count; i++) {
                    print_ast(node->as.arr.literals[i], indent + 2);
                }
            }
            break;

        case AST_FN_DECL:
            printf("FnDecl");
            print_location(node->location);
            printf("\n");
            print_indent(indent + 1);
            printf("Identifier: %s\n", node->as.func.ident->as.ident.name);
            
            print_indent(indent + 1);
            printf("Params (%zu):\n", node->as.func.params_count);
            for (size_t i = 0; i < node->as.func.params_count; i++) {
                print_ast(node->as.func.params[i], indent + 2);
            }
            
            print_indent(indent + 1);
            printf("ReturnType: %s\n",
                   node->as.func.return_type ? node->as.func.return_type->lexeme : "void");
            
            print_indent(indent + 1);
            printf("Body:\n");
            print_ast(node->as.func.block, indent + 2);
            break;

        case AST_PARAM:
            printf("Param(ident: %s, type: %s)",
                   node->as.param.ident->as.ident.name,
                   node->as.param.type->lexeme);
            print_location(node->location);
            printf("\n");
            break;

        // =================== TYPE DEFINITIONS ===================
        case AST_STRUCT:
            printf("Struct");
            print_location(node->location);
            printf("\n");
            print_indent(indent + 1);
            printf("Identifier: %s\n", node->as.structType.name->as.ident.name);
            print_indent(indent + 1);
            printf("Fields (%zu):\n", node->as.structType.fields_count);
            for (size_t i = 0; i < node->as.structType.fields_count; i++) {
                print_ast(node->as.structType.fields[i], indent + 2);
            }
            break;

        case AST_UNION:
            printf("Variant");
            print_location(node->location);
            printf("\n");
            print_indent(indent + 1);
            printf("Identifier: %s\n", node->as.unionType.name->as.ident.name);
            print_indent(indent + 1);
            printf("Fields (%zu):\n", node->as.unionType.fields_count);
            for (size_t i = 0; i < node->as.unionType.fields_count; i++) {
                print_ast(node->as.unionType.fields[i], indent + 2);
            }
            break;

        case AST_ENUM:
            printf("Enum");
            print_location(node->location);
            printf("\n");
            print_indent(indent + 1);
            printf("Identifier: %s\n", node->as.enumType.enum_name->as.ident.name);
            print_indent(indent + 1);
            printf("Values (%zu):\n", node->as.enumType.enum_count);
            for (size_t i = 0; i < node->as.enumType.enum_count; i++) {
                print_ast(node->as.enumType.enum_values[i], indent + 2);
            }
            break;

        case AST_FIELD:
            printf("Field(ident: %s, type: %s)",
                   node->as.field.ident->as.ident.name,
                   node->as.field.type->lexeme);
            print_location(node->location);
            printf("\n");
            break;

        // =================== CONTROL FLOW ===================
        case AST_IF:
            printf("IfStmt");
            print_location(node->location);
            printf("\n");
            
            print_indent(indent + 1);
            printf("Condition:\n");
            print_ast(node->as.ifstmt.condition, indent + 2);
            
            // Print then_branch
            print_indent(indent + 1);
            if (node->as.ifstmt.then_branch && 
                node->as.ifstmt.then_branch->type == AST_BLOCK) {
                Block* block = &node->as.ifstmt.then_branch->as.block;
                printf("Then (%zu statements):\n", block->count);
                for (size_t i = 0; i < block->count; i++) {
                    print_ast(block->statements[i], indent + 2);
                }
            } else {
                printf("Then:\n");
                print_ast(node->as.ifstmt.then_branch, indent + 2);
            }
            
            // Print else_branch if it exists
            if (node->as.ifstmt.else_branch) {
                print_indent(indent + 1);
                if (node->as.ifstmt.else_branch->type == AST_IF) {
                    printf("Else (else-if chain):\n");
                    print_ast(node->as.ifstmt.else_branch, indent + 2);
                } else if (node->as.ifstmt.else_branch->type == AST_BLOCK) {
                    Block* block = &node->as.ifstmt.else_branch->as.block;
                    printf("Else (%zu statements):\n", block->count);
                    for (size_t i = 0; i < block->count; i++) {
                        print_ast(block->statements[i], indent + 2);
                    }
                } else {
                    printf("Else:\n");
                    print_ast(node->as.ifstmt.else_branch, indent + 2);
                }
            }
            break;

        case AST_MATCH:
            printf("MatchStmt");
            print_location(node->location);
            printf("\n");
            print_indent(indent + 1);
            printf("Pattern:\n");
            print_ast(node->as.matchstmt.pattern, indent + 2);
            
            print_indent(indent + 1);
            printf("Cases (%zu):\n", node->as.matchstmt.case_count);
            for (size_t i = 0; i < node->as.matchstmt.case_count; i++) {
                print_ast(node->as.matchstmt.match_cases[i], indent + 2);
            }
            
            if (node->as.matchstmt.def_case) {
                print_indent(indent + 1);
                printf("Default:\n");
                print_ast(node->as.matchstmt.def_case, indent + 2);
            }
            break;

        case AST_MATCH_CASE:
            printf("MatchCase");
            print_location(node->location);
            printf("\n");
            print_indent(indent + 1);
            printf("Expr:\n");
            print_ast(node->as.matchcase.expr, indent + 2);
            print_indent(indent + 1);
            printf("Statement:\n");
            print_ast(node->as.matchcase.stmt, indent + 2);
            break;

        case AST_FOR:
            printf("ForLoop");
            print_location(node->location);
            printf("\n");
            print_indent(indent + 1);
            printf("Expr:\n");
            print_ast(node->as.forloop.expr, indent + 2);
            print_indent(indent + 1);
            printf("Body:\n");
            print_ast(node->as.forloop.block, indent + 2);
            break;

        case AST_LOOP:
            printf("Loop");
            print_location(node->location);
            printf("\n");
            print_indent(indent + 1);
            printf("Body:\n");
            print_ast(node->as.loop.block, indent + 2);
            break;

        case AST_RETURN:
            printf("ReturnStmt");
            print_location(node->location);
            printf("\n");
            if (node->as.return_stmt.expr) {
                print_ast(node->as.return_stmt.expr, indent + 1);
            }
            break;

        // =================== STRUCTURAL ===================
        case AST_BLOCK:
            printf("Block (%zu statements)", node->as.block.count);
            print_location(node->location);
            printf("\n");
            print_statements(node->as.block.statements, node->as.block.count, indent + 1);
            break;

        case AST_PROGRAM:
            printf("Program (%d statements)", node->as.program.stmt_count);
            print_location(node->location);
            printf("\n");
            print_statements(node->as.program.statements, node->as.program.stmt_count, indent + 1);
            break;

        // =================== NOT IMPLEMENTED ===================
        case AST_VECTOR:
            printf("Vector (not fully implemented)");
            print_location(node->location);
            printf("\n");
            break;

        case AST_STMT:
            printf("Stmt (generic statement node)");
            print_location(node->location);
            printf("\n");
            break;

        default:
            printf("Unknown AST node type: %d", node->type);
            print_location(node->location);
            printf("\n");
            break;
    }
}