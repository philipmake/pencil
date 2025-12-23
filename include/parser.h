#ifndef PARSER_H_
#define PARSER_H_

#include "ast.h"
#include <stdbool.h>
#include <stdio.h>

// define parser object
typedef struct 
{
    Token** tokens;
    int current;
    int count;
    const char* error_msg;
} Parser;

/* parser functions */
Parser* init_parser(Token** tokens, int count);
void backtrack(Parser* parser, int offset);
Token* parser_peek(Parser* parser);
Token* parser_previous(Parser* parser);
Token* parser_advance(Parser* parser);
Token* parser_current(Parser* parser);
bool parser_check(Parser* p, TokenType type);
bool parser_match(Parser* p, TokenType type);
Token* parser_consume(Parser* p, TokenType type, const char* message);
void parser_error(Parser* p, const char* message);


// program entry
ASTNode* parse_program(Parser* parser);

// declarations
ASTNode* parse_var_decl(Parser* parser);
ASTNode* parse_const_decl(Parser* parser);
ASTNode* parse_array_decl(Parser* parser, ASTNode* ident);
ASTNode* parse_array_init(Parser* parser, ASTNode* ident, Token* data_type);
ASTNode* parse_param(Parser* parser);
ASTNode* parse_func_decl(Parser* parser); 

// statements
ASTNode* parse_stmt(Parser* parser);
ASTNode* parse_expr_stmt(Parser* parser); 
ASTNode* parse_if_stmt(Parser* parser);
ASTNode* parse_match_stmt(Parser* parser); 
ASTNode* parse_for_stmt(Parser* parser); // not implemented
ASTNode* parse_loop_stmt(Parser* parser); 
ASTNode* parse_return_stmt(Parser* parser); 
ASTNode* parse_block(Parser* parser); 

// expressions
ASTNode* parse_expr(Parser* parser);
ASTNode* parse_for_expr(Parser* parser);
ASTNode* parse_assign_expr(Parser* parser);
ASTNode* parse_logical_or(Parser* parser); // logical_or top level
ASTNode* parse_logical_and(Parser* parser); // logical_or top level
ASTNode* parse_equality(Parser* parser);  // equality level, == or !=
ASTNode* parse_comparison(Parser* parser); // comparison level, <, >, <=, >=
ASTNode* parse_range(Parser* parser);
ASTNode* parse_add_expr(Parser* parser); // + -
ASTNode* parse_mult_expr(Parser* parser); // * / %
ASTNode* parse_unary_expr(Parser* parser);
ASTNode* parse_postfix_expr(Parser* parser); // handles [] and ()
ASTNode* parse_primary_expr(Parser* parser);

// data types
ASTNode* parse_enum(Parser* parser);
ASTNode* parse_struct(Parser* parser);
ASTNode* parse_union(Parser* parser);
ASTNode* parse_vec(Parser* parser);
ASTNode* parse_list(Parser* parser);

// Utilities
static inline bool parser_is_at_end(Parser* parser) 
{ 
    return parser->current >= parser->count 
    || parser->tokens[parser->current]->type == TOKEN_EOF; 
}

ASTNode** parser_grow_array(ASTNode** arr, size_t* count, ASTNode* elem);
ASTNode* create_program();
void add(ASTNode* parent, ASTNode* child);



#endif
