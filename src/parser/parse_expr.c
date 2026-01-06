#include "ast.h"
#include "parser.h"
#include "token.h"
#include "scope.h"
#include "symtab.h"
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
    // Peek ahead to see if this looks like an assignment
    if (parser_check(parser, IDENTIFIER))
    {
        // Look ahead one more token
        if (parser->current + 1 < parser->count)
        {
            Token* next = parser->tokens[parser->current + 1];
            
            // Check if next token is an assignment operator
            if (next->type == ASSIGN || next->type == PLUS_ASSIGN || 
                next->type == MINUS_ASSIGN || next->type == STAR_ASSIGN ||
                next->type == SLASH_ASSIGN || next->type == PERCENT_ASSIGN || 
                next->type == AND_ASSIGN)
            {
                // This IS an assignment
                Token* name = parser_advance(parser);  // consume identifier
                Token* op = parser_advance(parser);     // consume operator
                ASTNode* value = parse_assign_expr(parser); // right-associative
                return ast_new_assign(name, op, value);
            }
        }
    }
    
    // Not an assignment, parse as lower precedence expression
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

            // Look up the symbol
            sym_entry_t* sym = symtab_lookup(parser->symtab, tk->lexeme);
            if (!sym)
                fprintf(stderr, "Error at line %d: Undefined identifier '%s'\n",
                        tk->location.line, tk->lexeme);
            else
                // Add reference
                symtab_add_reference(sym, tk->location.line, 0);  // 0 = read

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

    parser_advance(parser);
    
    if (parser_check(parser, COLON)) {
        Token* identifier = parser_previous(parser);
        variable = ast_new_identifier(identifier);

        parser_match(parser, COLON);

        expr = parse_range(parser);
    } else {
        parser->current--; // backtrack here
        expr = parse_expr(parser);
    }

    return ast_loop_expr(variable, expr);
}


