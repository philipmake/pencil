#ifndef LEXER_H_
#define LEXER_H_
// lexer header file

#include "token.h"

#define EOF_CHAR ((char)-1)

typedef struct
{
    char *source;
    int line;
    int column;
    char *current_input_char;
    char *source_buffer;
} Lexer;

Lexer* lexer_init(char* filename);

void advance(Lexer *lexer);
char peek(Lexer *lexer, int offset);

void lexer(Lexer *lexer);   // main lexer loop

void lexer_skip_comments(Lexer *lexer);
void lexer_skip_whitespace(Lexer *lexer);
void lex_digits(Lexer *lexer);
void lex_alpha(Lexer *lexer);
void lex_string_literal(Lexer* lexer);
void lex_char_literal(Lexer* lexer);
void operators_and_delimiters(Lexer *lexer);

#endif