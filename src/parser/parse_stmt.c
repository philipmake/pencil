#include "ast.h"
#include "parser.h"
#include "token.h"
#include "scope.h"
#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


ASTNode* parse_block(Parser* parser)
{
    printf("DEBUG: Entering parse_block\n");
    
    if (!parser_match(parser, OPEN_CURLY)) {
        printf("DEBUG: No opening brace found\n");
        return NULL;
    }

    // Enter new scope
    if (parser->symtab != NULL) {
        printf("DEBUG: Entering block scope (current depth: %d)\n", 
               parser->symtab->current_depth);
        symtab_enter_scope(parser->symtab);
        printf("DEBUG: New depth: %d\n", parser->symtab->current_depth);
    } else {
        printf("DEBUG: WARNING - parser->symtab is NULL in parse_block\n");
    }

    ASTNode** stmts = NULL; 
    size_t stmt_count = 0;
    
    while (!parser_check(parser, CLOSE_CURLY))
    {
        while (parser_match(parser, NEWLINE)) { ; }

        if (parser_check(parser, CLOSE_CURLY)) break;
        
        printf("DEBUG: Parsing statement in block\n");
        ASTNode* stmt = parse_stmt(parser);
        if (!stmt) 
        {
            parser_error(parser, "Invalid statement in block");
            break;
        }
        stmts = parser_grow_array(stmts, &stmt_count, stmt);
    }
    
    parser_consume(parser, CLOSE_CURLY, "Expected a '}' at end of block\n");  
    
    // Exit scope
    if (parser->symtab != NULL) {
        printf("DEBUG: Exiting block scope (current depth: %d)\n", 
               parser->symtab->current_depth);
        symtab_exit_scope(parser->symtab);
        printf("DEBUG: New depth: %d\n", parser->symtab->current_depth);
    }
    
    printf("DEBUG: Exiting parse_block\n");
    return ast_block(stmts, stmt_count);
}
ASTNode* parse_return_stmt(Parser* parser)
{
    parser_advance(parser); // consume return keyword
    ASTNode* expr = parse_expr(parser);
    parser_consume(parser, SEMICOLON, "Expected a ';' after return statement\n");  
    return ast_return_stmt(expr);
}

ASTNode* parse_if_stmt(Parser* parser)
{
    if (!parser_consume(parser, IF, "Expected 'if' keyword"))
        return NULL;
    
    ASTNode* condition = parse_expr(parser);
    ASTNode* then_branch = parse_block(parser);  // Returns a Block node
    
    if (!then_branch) {
        parser_error(parser, "Expected block after if condition");
        return NULL;
    }
    
    ASTNode* else_branch = NULL;
    if (parser_match(parser, ELSE))
    {
        if (parser_check(parser, IF))
        {
            else_branch = parse_if_stmt(parser);  // Else-if
        } else {
            else_branch = parse_block(parser);  // Else block
        }
    }
    
    return ast_if(condition, then_branch, else_branch);
}

ASTNode* parse_match_stmt(Parser* parser)
{
    parser_consume(parser, MATCH, "Expected 'match' keyword.");
    ASTNode* pattern = parse_expr(parser);

    parser_consume(parser, OPEN_CURLY, "Expected '{' after match pattern.");
    ASTNode** match_cases = NULL;
    size_t case_count = 0;
    ASTNode* def_case = NULL;

    while (!parser_check(parser, CLOSE_CURLY) && !parser_is_at_end(parser))
    {
        if (parser_match(parser, UNDERSCORE)) 
        {            
            // Default case "_ => <expr>;"
            parser_consume(parser, ARROW, "Expected '=>' after '_'.");

            ASTNode* result_expr = parse_stmt(parser);
            // parser_consume(parser, SEMICOLON, "Expected ';' at end of default case.");

            def_case = ast_new_match_case(NULL, result_expr);
        }
        else 
        {
            // Normal case "<expr> => <expr>;"
            ASTNode* case_expr = parse_expr(parser);
            parser_consume(parser, ARROW, "Expected '=>' after case expression.");

            ASTNode* result_expr = parse_stmt(parser);
            // parser_consume(parser, SEMICOLON, "Expected ';' at end of case.");

            ASTNode* new_case = ast_new_match_case(case_expr, result_expr);
            match_cases = realloc(match_cases, sizeof(ASTNode*) * (case_count + 1));
            match_cases[case_count++] = new_case;
        }
    }

    parser_consume(parser, CLOSE_CURLY, "Expected '}' after match cases.");

    return ast_new_match_stmt(pattern, match_cases, case_count, def_case);
}


ASTNode* parse_loop_stmt(Parser* parser)
{
    parser_match(parser, LOOP);
    ASTNode* condition = NULL;
    if (parser_check(parser, OPEN_CURLY))
    {
        goto block;
    }
    else 
    {
        condition = parse_loop_expr(parser);
    }

    block:
    ASTNode* block = NULL;
    if (parser_check(parser, OPEN_CURLY))
        block = parse_block(parser);

    return ast_loop(condition, block);
}

ASTNode* parse_stmt(Parser* parser)
{
    // Skip leading newlines
    // while (parser_match(parser, NEWLINE)) { ; }
    
    if (parser_check(parser, VAR))
    {
        ASTNode* var_stmt = parse_var_decl(parser);
        if (var_stmt != NULL)
            return var_stmt;
    }

    if (parser_check(parser, LET))
    {
        ASTNode* let_stmt = parse_const_decl(parser);
        if (let_stmt != NULL)
            return let_stmt;
    }
    
    if (parser_check(parser, IF))
    {
        ASTNode* if_stmt = parse_if_stmt(parser);
        if (if_stmt != NULL) return if_stmt; 
    }

    if (parser_check(parser, MATCH))
    {
        ASTNode* match_stmt = parse_match_stmt(parser);
        if (match_stmt != NULL) return match_stmt;
    }

    if (parser_check(parser, FN))
    {
        ASTNode* fun_decl = parse_func_decl(parser);
        if (fun_decl != NULL) return fun_decl;
    }

    if (parser_check(parser, RETURN))
    {
        ASTNode* return_stmt = parse_return_stmt(parser);
        if (return_stmt != NULL)
            return return_stmt;
    }

    if (parser_check(parser, LOOP))
    {
        ASTNode* loopstmt = parse_loop_stmt(parser);
        if (loopstmt != NULL) return loopstmt; 
    }

    if (parser_check(parser, STRUCT))
    {
        ASTNode* struct_type = parse_struct(parser);
        if (struct_type != NULL) return struct_type;
    }

    if (parser_check(parser, UNION))
    {
        ASTNode* unionValue = parse_union(parser);
        if (unionValue != NULL) return unionValue;
    }

    if (parser_check(parser, ENUM))
    {
        ASTNode* enums = parse_enum(parser);
        if (enums != NULL) return enums;
    }

    // Otherwise treat as expression statement (assignments, function calls, etc.)
    ASTNode* exprStmt = parse_expr(parser);
    if (exprStmt != NULL)
        return exprStmt;

    return NULL;
}

