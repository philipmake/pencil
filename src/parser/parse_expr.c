#include "ast.h"
#include "parser.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


ASTNode* parse_expr(Parser* parser)
{
    return parse_assign_expr(parser);
}

// Parse assignment expressions: IDENTIFIER "=" expr
// Right-associative (a = b = c -> a = (b = c))
ASTNode* parse_assign_expr(Parser* parser)
{
    if (parser_check(parser, IDENTIFIER))
    {
        Token* name = parser_advance(parser);
        if (parser_check(parser, ASSIGN))
        {
            Token* op = parser_advance(parser);
            ASTNode* value = parse_assign_expr(parser);    // right assoc i.e a=b=c -> a=(b=c)
            return ast_new_assign(name, op, value);
        }
        
        parser->current--; // Backtrack
    }

    return parse_logical_or(parser);
}

// Parse logical OR expressions: expr "||" expr
ASTNode* parse_logical_or(Parser* parser)
{
    ASTNode* left = parse_logical_and(parser);

    while (parser_check(parser, OR))
    {
        Token* op = parser_advance(parser);
        ASTNode* right = parse_logical_and(parser);
        left = ast_new_binary(left, op, right);
    }

    return left;
} 

// Parse logical AND expressions: expr "&&" expr
ASTNode* parse_logical_and(Parser* parser)
{
    ASTNode* left = parse_equality(parser);

    while (parser_check(parser, AND))
    {
        Token* op = parser_advance(parser);
        ASTNode* right = parse_equality(parser);
        left = ast_new_binary(left, op, right);
    }

    return left;
} 

// Parse equality comparisons: expr "==" expr | expr "!=" expr
ASTNode* parse_equality(Parser* parser)
{
    ASTNode* left = parse_comparison(parser);

    while (parser_peek(parser)->type == EQUAL || parser_peek(parser)->type == NOT_EQUAL)
    {
        Token* op = parser_advance(parser);
        ASTNode* right = parse_comparison(parser);
        left = ast_new_binary(left, op, right);
    }

    return left;
}

// Parse relational comparisons: >, <, >=, <=
ASTNode* parse_comparison(Parser* parser)
{
    ASTNode* left = parse_range(parser);

    TokenType tp = parser_peek(parser)->type;
    while (tp == GREATER 
    || tp == LESS 
    || tp == GREATER_EQUAL
    || tp == LESS_EQUAL)
    {
        Token* op = parser_advance(parser);
        ASTNode* right = parse_range(parser);
        left = ast_new_binary(left, op, right);
        tp = parser_peek(parser)->type;  // Update for next iteration
    }

    return left;
}

ASTNode* parse_range(Parser* parser)
{
    ASTNode* start = parse_add_expr(parser);
    ASTNode* inc_end = NULL;
    ASTNode* step = NULL;

    // while stmt here ensures support for multiple ellipsis
    // e.g a...b...c
    while (parser_match(parser, ELLIPSIS))
    {
        ASTNode* end = parse_add_expr(parser);
        if (parser_match(parser, ELLIPSIS))
            step = parse_add_expr(parser);

        start = ast_new_range(start, end, step);
    }

    return start;
}

// Parse addition and subtraction: expr "+" expr | expr "-" expr
ASTNode* parse_add_expr(Parser* parser)
{
    ASTNode* left = parse_mult_expr(parser);

    while (parser_peek(parser)->type == PLUS || parser_peek(parser)->type == MINUS)
    {
        Token* op = parser_advance(parser);
        ASTNode* right = parse_mult_expr(parser);
        left = ast_new_binary(left, op, right);
    }

    return left;
}

// Parse multiplication and division: expr "*" expr | expr "/" expr
ASTNode* parse_mult_expr(Parser* parser)
{
    ASTNode* left = parse_unary_expr(parser);

    while (parser_peek(parser)->type == STAR || parser_peek(parser)->type == SLASH || parser_peek(parser)->type == PERCENT)
    {
        Token* op = parser_advance(parser);
        ASTNode* right = parse_unary_expr(parser);
        left = ast_new_binary(left, op, right);
    }
    return left;
}

// Parse unary operators: "!" expr | "-" expr
ASTNode* parse_unary_expr(Parser* parser)
{
    TokenType tp = parser_peek(parser)->type;
    if (tp == NOT || tp == MINUS)
    {
        Token* op = parser_advance(parser);
        ASTNode* operand = parse_unary_expr(parser);
        return ast_new_unary(op, operand);
    }

    return parse_postfix_expr(parser);
}

// foo[0] or foo(...)
ASTNode* parse_postfix_expr(Parser* parser)
{
    ASTNode* primary = parse_primary_expr(parser);

    while (true)
    {
        if (parser_match(parser, OPEN_BRACKET))
        {
            // Example: arr[1]
            ASTNode* index = parse_expr(parser);
            parser_consume(parser, CLOSE_BRACKET, "Expected ']' after index expression.");
            primary = ast_new_index(primary, index);
        }
        else if (parser_match(parser, OPEN_PAREN))
        {
            // Example: foo(1, 2)
            ASTNode** args = NULL;
            size_t count = 0;

            if (!parser_check(parser, CLOSE_PAREN))
            {
                do {
                    ASTNode* arg = parse_expr(parser);
                    args = parser_grow_array(args, &count, arg);
                } while (parser_match(parser, COMMA));
            }

            parser_consume(parser, CLOSE_PAREN, "Expected ')' after arguments.");
            primary = ast_new_call(primary, args, count);
        }
        else
        {
            break; // no more postfix operators
        }
    }

    return primary;
}


// Parse primary expressions: literals, identifiers, or parenthesized expressions.
ASTNode* parse_primary_expr(Parser* parser)
{
    Token* tk;
    TokenType tp = parser_peek(parser)->type;
    switch(tp)
    {
        case INT_LITERAL:
        case FLOAT_LITERAL:
        case CHAR_LITERAL:
        case BOOL_LITERAL:
        case HEX_LITERAL:
        case OCTAL_LITERAL:
        case BINARY_LITERAL:
        case STRING_LITERAL:
            tk = parser_advance(parser);
            return ast_new_literal(tk);
        case IDENTIFIER:
            tk = parser_advance(parser);
            return ast_new_identifier(tk);
        case OPEN_PAREN: 
        {
            parser_advance(parser);
            ASTNode* expr = parse_expr(parser);
            parser_consume(parser, CLOSE_PAREN, "Expected closing ')' after expression");
            return expr;
        }
           
        default:
            parser_error(parser, "Expected expression");
            return NULL; 
    }
}


ASTNode* parse_loop_expr(Parser* parser)
{
    ASTNode* variable = NULL;
    ASTNode* expr = NULL;
    
    /* check for idenifier and that next token is : else parse expression.
    if NULL take as a loop with no condtion.
    format => variable : expression
    */
    if (parser_match(parser, IDENTIFIER))
    {
        Token* idenifier = parser_advance(parser); 
        variable = ast_new_identifier(idenifier);
        parser_match(parser, SEMICOLON);
    } else 
        goto expr_start_loop_condtion;
    
    expr_start_loop_condtion:
    expr = parse_range(parser);

    return ast_loop_expr(variable, expr);
}