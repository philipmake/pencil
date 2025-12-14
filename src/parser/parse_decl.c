#include "ast.h"
#include "parser.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


ASTNode* parse_var_decl(Parser* parser)
{
    parser_consume(parser, VAR, "Expected 'var' keyword");

    Token* ident_tk = parser_consume(parser, IDENTIFIER, "Expected identifier after 'var'");
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
        else
            parser_error(parser, "Expected type after ':'\n");
    }

    // Handle initializer
    if (parser_match(parser, ASSIGN))
        value = parse_expr(parser);

    parser_consume(parser, NEWLINE, "Expected newline \n");

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
        else
            parser_error(parser, "Expected type afer':'\n");
    }

    // Handle initializer
    if (parser_match(parser, ASSIGN))
        value = parse_expr(parser);

    parser_consume(parser, NEWLINE, "Expected newline\n");
    return ast_const_decl(ident, data_type, value);
}

ASTNode* parse_param(Parser* parser)
{
    Token* ident_tk = parser_advance(parser);
    ASTNode* ident = ast_new_identifier(ident_tk);

    parser_consume(parser, COLON, "Expected ':' after parameter name.\n");

    Token* type = parser_advance(parser);
    return ast_param(ident, type);
}

ASTNode* parse_func_decl(Parser* parser) 
{
    ASTNode* identifier = NULL;
    ASTNode** params = NULL;
    size_t params_count = 0;
    ASTNode* block = NULL;
    Token* return_type = NULL;

    if (!parser_match(parser, FN))
        return NULL;
    
    Token* ident = parser_advance(parser);
    identifier = ast_new_identifier(ident);

    parser_consume(parser, OPEN_PAREN, "Expected '(' after function name\n");
    
    if (!parser_check(parser, CLOSE_PAREN)) 
    {
        do {
            ASTNode* param = parse_param(parser);
            params = parser_grow_array(params, &params_count, param);
        } while (parser_match(parser, COMMA));
    }
    parser_consume(parser, CLOSE_PAREN, "Expected ')' after parameters\n");

    // return type
    if (parser_match(parser, ARROW))
    {
        return_type = parser_advance(parser);
    }

    // parser block
    if (parser_check(parser, OPEN_CURLY))
    {
        block = parse_block(parser);
    }
    
    return ast_fn_decl(identifier, params, params_count, return_type, block);
}

