#include "ast.h"
#include "parser.h"
#include "token.h"
#include "scope.h"
#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


ASTNode* parse_var_decl(Parser* parser)
{
    printf("DEBUG: Entering parse_var_decl\n");
    parser_consume(parser, VAR, "Expected 'var' keyword");

    Token* ident_tk = parser_consume(parser, IDENTIFIER, "Expected identifier after 'var'");
    if (!ident_tk) {
        printf("DEBUG: Failed to get identifier token\n");
        return NULL;
    }
    printf("DEBUG: Got identifier: %s\n", ident_tk->lexeme);
    
    ASTNode* ident = ast_new_identifier(ident_tk);

    Token* data_type = NULL;
    ASTNode* value = NULL;

    // Check for array declaration -> var ident[...]
    if (parser_match(parser, OPEN_BRACKET)) 
        return parse_array_decl(parser, ident);

    // Handle typed declaration -> var ident: Type
    if (parser_match(parser, COLON))
    {
        if (parser_check(parser, TYPE) || parser_check(parser, IDENTIFIER))
            data_type = parser_advance(parser);
    }

    // Handle initializer
    if (parser_match(parser, ASSIGN))
        value = parse_expr(parser);
    
    parser_match(parser, NEWLINE);

    // SYMBOL TABLE INSERTION - ADD NULL CHECKS
    if (parser->symtab == NULL) {
        printf("DEBUG: ERROR - parser->symtab is NULL!\n");
        return ast_var_decl(ident, data_type, value);
    }
    
    printf("DEBUG: Checking for redeclaration of '%s'\n", ident_tk->lexeme);
    
    // Check for redeclaration in current scope
    if (symtab_lookup_current_scope(parser->symtab, ident_tk->lexeme)) {
        fprintf(stderr, "Error at line %d: Variable '%s' already declared in this scope\n",
                ident_tk->location.line, ident_tk->lexeme);
        // Don't return NULL, just warn and continue
    } else {
        printf("DEBUG: Creating symbol for '%s'\n", ident_tk->lexeme);
        
        // Create and insert symbol
        sym_entry_t* sym = sym_create(
            ident_tk->lexeme,
            SYM_VARIABLE,
            TYPE_UNKNOWN,  // We'll improve type inference later
            ident_tk->location.line
        );
        
        if (!sym) {
            printf("DEBUG: ERROR - sym_create returned NULL!\n");
        } else {
            printf("DEBUG: Symbol created, inserting into table\n");
            sym->level = parser->symtab->current_depth;
            sym->scope = parser->symtab->current_scope;
            
            if (symtab_insert(parser->symtab, sym) == NULL) {
                printf("DEBUG: ERROR - symtab_insert failed!\n");
            } else {
                printf("DEBUG: Symbol inserted successfully\n");
            }
        }
    }

    printf("DEBUG: Exiting parse_var_decl\n");
    return ast_var_decl(ident, data_type, value);
}

ASTNode* parse_array_decl(Parser* parser, ASTNode* ident)
{
    Token* data_type = NULL;
    ASTNode* range = NULL;

    if (parser_check(parser, TYPE))
    {
        data_type = parser_advance(parser);
        if (parser_consume(parser, COLON, "Expected a ':' after array type\n"))
            range = ast_new_literal(parser_advance(parser));
    }

    parser_consume(parser, CLOSE_BRACKET, "Expected a ']' after array size\n");

    parser_consume(parser, ASSIGN, "Expected '=' after array declaration\n");
    parser_consume(parser, OPEN_BRACKET, "Expected '[' to begin array initializer\n");

    ASTNode** elements = NULL;
    size_t count = 0;
    if (!parser_check(parser, CLOSE_BRACKET))
    {
        do 
        {
            ASTNode* literal = parse_expr(parser);
            elements = parser_grow_array(elements, &count, literal);
        } while (parser_match(parser, COMMA));
    }

    parser_consume(parser, CLOSE_BRACKET, "Expected a ']' after array initialization\n");
    parser_consume(parser, NEWLINE, "Expected newline\n");

    return ast_new_array(ident, data_type, range, elements, count);
}

ASTNode* parse_const_decl(Parser* parser)
{
    if (!parser_consume(parser, LET, "Expected 'let' keyword\n"))
        return NULL;

    Token* ident_tk = parser_consume(parser, IDENTIFIER, "Expected identifier after 'let'\n");
    ASTNode* ident = ast_new_identifier(ident_tk);

    Token* data_type = NULL;
    ASTNode* value = NULL;

    // Check for array declaration -> let ident[...]
    if (parser_match(parser, OPEN_BRACKET)) 
        return parse_array_decl(parser, ident);

    // Handle typed declaration -> let ident: Type
    if (parser_match(parser, COLON))
    {
        if (parser_check(parser, TYPE) || parser_check(parser, IDENTIFIER))
            data_type = parser_advance(parser);
    }

    // Handle initializer
    if (parser_match(parser, ASSIGN))
        value = parse_expr(parser);

    parser_match(parser, NEWLINE);

    // create and insert symbol
    sym_entry_t* sym = sym_create(
        ident_tk->lexeme,
        SYM_VARIABLE,
        TYPE_UNKNOWN,
        ident_tk->location.line
    );
    // set level and scope
    sym->level = parser->symtab->current_depth;
    sym->scope = parser->symtab->current_scope;

    symtab_insert(parser->symtab, sym);
    
    return ast_const_decl(ident, data_type, value);
}

ASTNode* parse_param(Parser* parser)
{
    Token* ident_tk = parser_advance(parser);
    ASTNode* ident = ast_new_identifier(ident_tk);

    parser_consume(parser, COLON, "Expected ':' after parameter name.\n");

    Token* type = parser_advance(parser);

    // SYMBOL TABLE CHECK
    if (parser->symtab == NULL) {
        printf("DEBUG: ERROR - parser->symtab is NULL in parse_func_decl!\n");
        return NULL;
    }

    // Check for redeclaration
    if (symtab_lookup_current_scope(parser->symtab, ident_tk->lexeme)) {
        fprintf(stderr, "Error: Function '%s' already declared\n", ident_tk->lexeme);
        // Continue parsing anyway
    } else {
        printf("DEBUG: Creating function symbol for '%s'\n", ident_tk->lexeme);
        
        // Create function symbol
        sym_entry_t* param_sym = sym_create(
            ident_tk->lexeme,
            SYM_PARAM,
            TYPE_VOID,
            ident->location.line
        );
        
        if (!param_sym) {
            printf("DEBUG: ERROR - Failed to create function symbol\n");
        } else {
            param_sym->info.param.position = 0;
            param_sym->info.param.offset = 0;
            
            if (symtab_insert(parser->symtab, param_sym) == NULL) {
                printf("DEBUG: ERROR - Failed to insert function symbol\n");
            } else {
                printf("DEBUG: Function symbol inserted successfully\n");
            }
        }
    }
    return ast_param(ident, type);
}

ASTNode* parse_func_decl(Parser* parser) 
{
    printf("DEBUG: Entering parse_func_decl\n");
    
    ASTNode* identifier = NULL;
    ASTNode** params = NULL;
    size_t params_count = 0;
    ASTNode* block = NULL;
    Token* return_type = NULL;

    if (!parser_match(parser, FN))
        return NULL;
    
    Token* ident = parser_advance(parser);
    if (!ident) {
        printf("DEBUG: Failed to get function identifier\n");
        return NULL;
    }
    printf("DEBUG: Got function name: %s\n", ident->lexeme);
    
    identifier = ast_new_identifier(ident);

    // SYMBOL TABLE CHECK
    if (parser->symtab == NULL) {
        printf("DEBUG: ERROR - parser->symtab is NULL in parse_func_decl!\n");
        goto skip_symtab;
    }

    // Check for redeclaration
    if (symtab_lookup_current_scope(parser->symtab, ident->lexeme)) {
        fprintf(stderr, "Error: Function '%s' already declared\n", ident->lexeme);
        // Continue parsing anyway
    } else {
        printf("DEBUG: Creating function symbol for '%s'\n", ident->lexeme);
        
        // Create function symbol
        sym_entry_t* func_sym = sym_create(
            ident->lexeme,
            SYM_FUNCTION,
            TYPE_VOID,
            ident->location.line
        );
        
        if (!func_sym) {
            printf("DEBUG: ERROR - Failed to create function symbol\n");
        } else {
            func_sym->info.func.param_count = 0;
            func_sym->info.func.params = NULL;
            func_sym->info.func.is_defined = 0;
            
            if (symtab_insert(parser->symtab, func_sym) == NULL) {
                printf("DEBUG: ERROR - Failed to insert function symbol\n");
            } else {
                printf("DEBUG: Function symbol inserted successfully\n");
            }
        }
    }

skip_symtab:
    parser_consume(parser, OPEN_PAREN, "Expected '(' after function name\n");
    
    // Enter function scope for parameters
    if (parser->symtab != NULL) {
        printf("DEBUG: Entering function scope\n");
        symtab_enter_scope(parser->symtab);
        if (parser->symtab->current_scope) {
            parser->symtab->current_scope->flags |= FUNCTION;
        }
    }
    
    if (!parser_check(parser, CLOSE_PAREN)) 
    {
        do {
            ASTNode* param = parse_param(parser);
            if (param) {
                params = parser_grow_array(params, &params_count, param);
            }
        } while (parser_match(parser, COMMA));
    }
    parser_consume(parser, CLOSE_PAREN, "Expected ')' after parameters\n");

    // return type
    if (parser_match(parser, ARROW))
    {
        return_type = parser_advance(parser);
    }

    // parse block
    if (parser_check(parser, OPEN_CURLY))
    {
        block = parse_block(parser);
    }
    
    // Exit function scope
    if (parser->symtab != NULL) {
        printf("DEBUG: Exiting function scope\n");
        symtab_exit_scope(parser->symtab);
    }
    
    printf("DEBUG: Exiting parse_func_decl\n");
    return ast_fn_decl(identifier, params, params_count, return_type, block);
}

