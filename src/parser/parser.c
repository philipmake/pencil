#include "ast.h"
#include "parser.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Initialize a new parser instance.
// Allocates memory for the Parser struct, sets the token array,
// resets the current index to 0, and clears error messages.
Parser* init_parser(Token** tokens, int count)
{
    Parser* parser =  (Parser *)malloc(sizeof(Parser));
    parser->tokens = tokens;
    parser->current = 0;
    parser->count = count;
    parser->error_msg = "";

    printf("\nParser initialized....\n");
    printf("Parser got %d tokens.\n", parser->count);

    // Debug: print tokens to ensure they're valid
    for (int i = 0; i < parser->count; i++) {
        if (parser->tokens[i])
            printf("Token[%d]: %s\n", i, parser->tokens[i]->lexeme);
    }

    return parser;
}

// Return the current token without advancing.
// If the parser is at the end, returns TOKEN_EOF.
Token* parser_peek(Parser* parser)
{
    return parser->tokens[parser->current];
}

// Return the most recently consumed token (one before current).
Token* parser_previous(Parser *parser)
{
    return parser->tokens[parser->current - 1];
}

// Advance to the next token in the stream and return the previous token.
// If already at the end, stays there
Token* parser_advance(Parser* parser)
{
    if (!(parser_is_at_end(parser)))
        parser->current++;

    return parser_previous(parser);
}

// Check if the current token is of a given type without consuming it
bool parser_check(Parser* parser, TokenType type)
{
    if(parser_is_at_end(parser))
        return false;
    
    return parser_peek(parser)->type == type;
}

// If the current token matches the expected type, consume it and return true.
// Otherwise, return false without consuming.
bool parser_match(Parser* parser, TokenType type)
{
    if (parser_check(parser, type))
    {
        parser_advance(parser);
        return true;
    }
    return false;
}

// Expect a token of a given type, consuming it if present.
// If not found, report an error and return an error token
Token* parser_consume(Parser* parser, TokenType type, const char* message)
{
    if (parser_check(parser, type))
        return parser_advance(parser);
    
    parser_error(parser, message);
    return NULL;
}

// Record an error message inside the parser state.
void parser_error(Parser* parser, const char* message) 
{ 
    parser->error_msg = message; 
}

// Print an error message with filename, line, and column information
void report_error(SourceLocation loc, const char* msg) 
{
    printf("Error at %s:%d:%d: %s\n",
        loc.filename ? loc.filename : "<stdin>",
        loc.line, loc.column, msg);
}


ASTNode* parse_program(Parser* parser)
{
    ASTNode* root = ast_program();

    while (!parser_is_at_end(parser))
    {
        ASTNode* stmt = parse_stmt(parser);   
        // append to the root
        if (stmt != NULL)
        {
            add_stmt(root, stmt);
        }
        else
        {
            parser_advance(parser);
        }
    }

    return root;
}


// Helper: grow dynamic array of ASTNode*
ASTNode** parser_grow_array(ASTNode** arr, size_t* count, ASTNode* elem) 
{
    arr = realloc(arr, sizeof(ASTNode*) * (*count + 1));
    if (!arr) 
    {
        fprintf(stderr, "Out of memory while growing ASTNode array\n");
        exit(1);
    }
    arr[*count] = elem;
    (*count)++;
    return arr;
}
