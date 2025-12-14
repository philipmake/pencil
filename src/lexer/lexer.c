#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"
#include "token.h"

#define TRUE 1
#define FALSE 0

Lexer* lexer_init(char* filename)
{
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    lexer->source = filename;
    lexer->line = 1;
    lexer->column = 1;

    // load source file and initialise current_input_char and source_buffer
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("error opening file: %s\n", filename);
        exit(1);
    }

    /* Peek first three bytes for BOM */
    int b1 = fgetc(file), b2 = fgetc(file), b3 = fgetc(file);
    if (!(b1 == 0xEF && b2 == 0xBB && b3 == 0xBF)) {
        /* not a BOM: rewind so we don’t lose data */
        fseek(file, 0, SEEK_SET);
    }

    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    rewind(file);

    // allocate buffer
    char *buf = (char *)malloc(fsize+1);
    if (buf == NULL)
    {
        printf("Memory allocation failed.\n");
        fclose(file);
        exit(1);
    }

    // read into memory
    fread(buf, 1, fsize, file);
    fclose(file);
    buf[fsize] = '\0';

    lexer->source_buffer = buf;
    lexer->current_input_char = lexer->source_buffer;

    printf("Lexer initialised for the file %s\n", lexer->source);
    return lexer;
}

void advance(Lexer *lexer)
{
    if (*lexer->current_input_char == '\n')
    {
        add_token(NEWLINE, "newline", 7, lexer->line, lexer->column);
        lexer->line++;
        lexer->column = 1;
    } else 
    {
        lexer->column++;
    }
    lexer->current_input_char++;
}

char peek(Lexer *lexer, int offset)
{
    if (*(lexer->current_input_char + offset) == '\0')
    {
        return EOF_CHAR;
    }
    return *(lexer->current_input_char + offset);
}

void lexer_skip_comments(Lexer *lexer)
{
    char ch = *(lexer->current_input_char);
    if (ch == '/')
    {
        if (peek(lexer, 1) == '/')
        {
            while(ch != '\n' && ch != '\0')
            {
                advance(lexer);
                ch = *(lexer->current_input_char);
            }
            if (ch == '\n') advance(lexer);  // (NOTE: only if newline) | consume the newline (if not EOF)
        }
        else if (peek(lexer, 1) == '*')
        {
            while(1)
            {
                advance(lexer);
                ch = *(lexer->current_input_char);
                if (ch == '\0')
                {
                    printf("Error: Unclosed block comment.\n");
                    exit(1);
                }

                if (peek(lexer, 1) == '*' && peek(lexer, 2) == '/')
                {
                    advance(lexer); advance(lexer);
                    break;
                }
            }

            advance(lexer);
        }
        else
        {
            operators_and_delimiters(lexer);
        }

        // else it's a division operator or SLASH

    }
}

void lex_digits(Lexer *lexer)
{
    char ch = *(lexer->current_input_char);
    if (ch == '0')
    {
        int has_point = FALSE;
        char next = peek(lexer, 1);
            
        if (next == 'b' || next == 'B') 
        {
            advance(lexer);
            advance(lexer); // skip the prefix -> '0b'
            int j = 0;
            while(peek(lexer, j) == '0' || peek(lexer, j) == '1')
            {
                j++;
            }
            char lexeme[j+1];
            for (int k = 0; k < j; k++)
            {
                lexeme[k] = *(lexer->current_input_char);
                advance(lexer);
            }
            lexeme[j] = '\0';
            add_token(BINARY_LITERAL, lexeme, j, lexer->line, lexer->column-j);
            return;
        } 
            
        if (next == 'o' || next == 'O') 
        {
            advance(lexer);
            advance(lexer); // skip the prefix -> '0o'
            int j = 0;
            while(peek(lexer, j) >= '0' && peek(lexer, j) <= '7')
            {
                j++;
            }
            char lexeme[j+1];
            for (int k = 0; k < j; k++)
            {
                lexeme[k] = *(lexer->current_input_char);
                advance(lexer);
            }
            lexeme[j] = '\0';
            add_token(OCTAL_LITERAL, lexeme, j, lexer->line, lexer->column-j);
            return;
        } 
            
        if (next == 'x' || next == 'X') 
        {
            advance(lexer);
            advance(lexer); // skip the prefix -> '0x'
            int j = 0;
            while(isxdigit(peek(lexer, j)))  // Using isxdigit() for cleaner hex checking
            {
                j++;
            }
            char lexeme[j+1];
            for (int k = 0; k < j; k++)
            {
                lexeme[k] = *(lexer->current_input_char);
                advance(lexer);
            }
            lexeme[j] = '\0';
            add_token(HEX_LITERAL, lexeme, j, lexer->line, lexer->column-j);
            return;
        }

        // Handle regular numbers starting with 0 (including 0.5, 007, etc.)
        int i = 0;  // Start from current position
        char curr = ch;  // Start with the '0' we already have
        
        while(1)
        {
            if (isdigit(curr))
            {
                i++;
                curr = peek(lexer, i);
            }
            else if (curr == '.' && !has_point && isdigit(peek(lexer, i+1)))
            {
                has_point = TRUE;
                i++;
                curr = peek(lexer, i);
            }
            else
            {
                break;
            }
        }
            
        char lexeme[i+1];
        for (int k = 0; k < i; k++)
        {
            lexeme[k] = *(lexer->current_input_char);
            advance(lexer);
        }
        lexeme[i] = '\0';
        add_token(has_point ? FLOAT_LITERAL : INT_LITERAL, lexeme, i, lexer->line, lexer->column-i);
    }
    else
    {
        // Handle numbers not starting with 0
        int i = 0;
        int has_point = FALSE;
        char curr = ch;
        
        while(1)
        {
            if (isdigit(curr))
            {
                i++;
                curr = peek(lexer, i);
            }
            else if (curr == '.' && !has_point && isdigit(peek(lexer, i+1)))
            {
                has_point = TRUE;
                i++;
                curr = peek(lexer, i);
            }
            else
            {
                break;
            }
        }
        
        char lexeme[i+1];
        for (int k = 0; k < i; k++)
        {
            lexeme[k] = *(lexer->current_input_char);
            advance(lexer);
        }
        lexeme[i] = '\0';
        add_token(has_point ? FLOAT_LITERAL : INT_LITERAL, lexeme, i, lexer->line, lexer->column-i);

    }
}

void lex_alpha(Lexer *lexer)
{
    int i = 0;
    while (isalnum(peek(lexer, i)) || peek(lexer, i) == '_') {
        i++;
    }

    char lexeme[i+1];
    for (int k = 0; k < i; k++)
    {
        lexeme[k] = *lexer->current_input_char;
        advance(lexer);
    }

    lexeme[i] ='\0';
    if (is_keyword(lexeme))
    {
        add_token(keyword_to_token(lexeme), lexeme, i, lexer->line, lexer->column-i);
    } else
    {
        add_token(IDENTIFIER, lexeme, i, lexer->line, lexer->column-i);
    }
}

void lex_string_literal(Lexer* lexer)
{
    char quote = *(lexer->current_input_char);  // first character to signal a string literal
    advance(lexer); // skip opening quote, no need " and ' handles it before jumping here
    int i = 0;
    while (peek(lexer, i) != quote && peek(lexer, i) != '\0') {
        i++;
    }

    if (peek(lexer, i) == '\0') {
        fprintf(stderr, "Unterminated string/char literal. Line %d, Column %d\n", lexer->line, lexer->column);
        exit(1);
    }

    char* lexeme = malloc(i + 1);
    for (int k = 0; k < i; k++) {
        lexeme[k] = *lexer->current_input_char;
        advance(lexer);
    }
    lexeme[i] = '\0';
    advance(lexer); // skip closing quote

    add_token(STRING_LITERAL, lexeme, i, lexer->line, lexer->column);
    free(lexeme);
}

void lex_char_literal(Lexer* lexer) 
{
    advance(lexer); // skip opening '
    char c = *lexer->current_input_char;

    if (c == '\\') {  // escape sequence
        advance(lexer);
        c = *lexer->current_input_char; 
    }
    else
    {
        c = *lexer->current_input_char;
    }

    char lexeme[2] = { c, '\0' };
    advance(lexer);

    if (*lexer->current_input_char != '\'') {
        fprintf(stderr, "Unterminated char literal at line %d, col %d\n", lexer->line, lexer->column);
        exit(1);
    }
    advance(lexer); // skip closing '

    add_token(CHAR_LITERAL, lexeme, 1, lexer->line, lexer->column - 1);
}


void operators_and_delimiters(Lexer *lexer)
{ 
    char ch = *(lexer->current_input_char);
    switch(ch)
        {
            case '"': 
                lex_string_literal(lexer); 
                break;
            case '\'':
                lex_char_literal(lexer);
                break;
            case '(': 
                add_token(OPEN_PAREN, "(", 1, lexer->line, lexer->column); 
                advance(lexer); 
                break;
            case ')': 
                add_token(CLOSE_PAREN, ")", 1, lexer->line, lexer->column); 
                advance(lexer); 
                break;
            case '{': 
                add_token(OPEN_CURLY, "{", 1, lexer->line, lexer->column); 
                advance(lexer); 
                break;
            case '}': 
                add_token(CLOSE_CURLY, "}", 1, lexer->line, lexer->column); 
                advance(lexer); 
                break;
            case '[': 
                add_token(OPEN_BRACKET, "[", 1, lexer->line, lexer->column); 
                advance(lexer); 
                break;
            case ']': 
                add_token(CLOSE_BRACKET, "]", 1, lexer->line, lexer->column); 
                advance(lexer); 
                break;
            case '_': 
                add_token(UNDERSCORE, "_", 1, lexer->line, lexer->column); 
                advance(lexer); 
                break;
            case '>':
                if (peek(lexer, 1) == '>')
                {
                    add_token(RSHIFT, ">>", 2, lexer->line, lexer->column);
                    advance(lexer);
                    advance(lexer);
                    break;
                } 
                if (peek(lexer, 1) == '=')
                {
                    add_token(GREATER_EQUAL, ">=", 2, lexer->line, lexer->column);
                    advance(lexer);
                    advance(lexer);
                    break;
                }
                else
                {
                    add_token(GREATER, ">", 1, lexer->line, lexer->column);
                    advance(lexer);
                    break;
                }

            case '<':
                if (peek(lexer, 1) == '<')
                {
                    add_token(LSHIFT, "<<", 2, lexer->line, lexer->column);
                    advance(lexer);
                    advance(lexer);
                    break;
                } 
                if (peek(lexer, 1) == '=')
                {
                    add_token(LESS_EQUAL, "<=", 2, lexer->line, lexer->column);
                    advance(lexer);
                    advance(lexer);
                    break;
                }
                else
                {
                    add_token(LESS, "<", 1, lexer->line, lexer->column);
                    advance(lexer);
                    break;
                }
            case ';': 
                add_token(SEMICOLON, ";", 1, lexer->line, lexer->column); 
                advance(lexer); 
                break;
            case ':': 
                add_token(COLON, ":", 1, lexer->line, lexer->column); 
                advance(lexer); 
                break;
            case ',': 
                add_token(COMMA, ",", 1, lexer->line, lexer->column); 
                advance(lexer); 
                break;
            case '.':
                if (peek(lexer, 1) == '.' && peek(lexer,2) == '.') 
                {
                    add_token(ELLIPSIS, "...", 3, lexer->line, lexer->column);
                    advance(lexer); advance(lexer); advance(lexer);
                    break;
                } else 
                {
                    add_token(DOT, ".", 1, lexer->line, lexer->column);
                    advance(lexer);
                    break;
                }
            case '=':
                if (peek(lexer, 1) == '=')
                {
                    add_token(EQUAL, "==", 2, lexer->line, lexer->column);
                    advance(lexer);
                    advance(lexer);
                    break;
                } else if (peek(lexer, 1) == '>')
                {
                    add_token(ARROW, "=>", 2, lexer->line, lexer->column);
                    advance(lexer);
                    advance(lexer);
                    break;
                } else 
                {
                    add_token(ASSIGN, "=", 1, lexer->line, lexer->column); 
                    advance(lexer); 
                    break;
                }
            case '/':
                if (peek(lexer, 1) == '=')
                {
                    add_token(SLASH_ASSIGN, "/=", 2, lexer->line, lexer->column);
                    advance(lexer);
                    advance(lexer);
                    break;
                } else 
                {
                    add_token(SLASH, "/", 1, lexer->line, lexer->column); 
                    advance(lexer); 
                    break;
                }
            case '%':
                if (peek(lexer, 1) == '=')
                {
                    add_token(PERCENT_ASSIGN, "%=", 2, lexer->line, lexer->column);
                    advance(lexer);
                    advance(lexer);
                    break;
                } else 
                {
                    add_token(PERCENT, "%", 1, lexer->line, lexer->column); 
                    advance(lexer); 
                    break;
                }
            case '-':
                if (peek(lexer, 1) == '-')
                {
                    add_token(MINUS_MINUS, "--", 2, lexer->line, lexer->column);
                    advance(lexer);
                    advance(lexer);
                    break;
                }
                if (peek(lexer, 1) == '=')
                {
                    add_token(MINUS_ASSIGN, "-=", 2, lexer->line, lexer->column);
                    advance(lexer);
                    advance(lexer);
                    break;
                }
                if (peek(lexer, 1) == '>')
                {
                    add_token(ARROW, "->", 2, lexer->line, lexer->column);
                    advance(lexer);
                    advance(lexer);
                    break;
                } else 
                {
                    add_token(MINUS, "-", 1, lexer->line, lexer->column); 
                    advance(lexer); 
                    break;
                }
            case '+':
                if (peek(lexer, 1) == '+')
                {
                    add_token(PLUS_PLUS, "++", 2, lexer->line, lexer->column);
                    advance(lexer);
                    advance(lexer);
                    break;
                }
                if (peek(lexer, 1) == '=')
                {
                    add_token(PLUS_ASSIGN, "+=", 2, lexer->line, lexer->column);
                    advance(lexer);
                    advance(lexer);
                    break;
                } else 
                {
                    add_token(PLUS, "+", 1, lexer->line, lexer->column); 
                    advance(lexer); 
                    break;
                }
            case '*':
                if (peek(lexer, 1) == '*')
                {
                    add_token(STAR_STAR, "**", 2, lexer->line, lexer->column);
                    advance(lexer);
                    advance(lexer);
                    break;
                }
                if (peek(lexer, 1) == '=')
                {
                    add_token(STAR_ASSIGN, "*=", 2, lexer->line, lexer->column);
                    advance(lexer);
                    advance(lexer);
                    break;
                } else 
                {
                    add_token(STAR, "*", 1, lexer->line, lexer->column); 
                    advance(lexer); 
                    break;
                }
            case '&':
                if (peek(lexer, 1) == '&')
                {
                    add_token(AND, "&&", 2, lexer->line, lexer->column);
                    advance(lexer);
                    advance(lexer);
                    break;
                }
                if (peek(lexer, 1) == '=')
                {
                    add_token(AND_ASSIGN, "&=", 2, lexer->line, lexer->column);
                    advance(lexer);
                    advance(lexer);
                    break;
                } else 
                {
                    add_token(BITWISE_AND, "&", 1, lexer->line, lexer->column); 
                    advance(lexer); 
                    break;
                }
            case '!':
                if (peek(lexer, 1) == '=')
                {
                    add_token(NOT_EQUAL, "!=", 2, lexer->line, lexer->column);
                    advance(lexer);
                    advance(lexer);
                    break;
                } else{
                    add_token(NOT, "!", 1, lexer->line, lexer->column);
                    advance(lexer);
                    break;
                }
            case '|':
                if (peek(lexer, 1) == '|')
                {
                    add_token(OR, "||", 2, lexer->line, lexer->column);
                    advance(lexer);
                    advance(lexer);
                    break;
                } else{
                    add_token(BITWISE_OR, "|", 1, lexer->line, lexer->column);
                    advance(lexer);
                    break;
                }
            default: 
            {
                advance(lexer);
                break;
            }
        }
}

void lexer(Lexer *lexer)
{
    while (*(lexer->current_input_char) != '\0')
    {
        char c = *(lexer->current_input_char);

        if (isspace(c))
        {
            advance(lexer);
            continue;
        }

        if (c == '/')
        {
            lexer_skip_comments(lexer);
            continue;
        }

        if (isalpha(c) || (c == '_' && isalnum(peek(lexer, 1))))
        {
            lex_alpha(lexer);
            continue;
        }

        if (isdigit(c))
        {
            lex_digits(lexer);
            continue;
        }

        // String or char literals
        if (c == '"' || c == '\'')
        {
            if (c == '"') lex_string_literal(lexer);
            else          lex_char_literal(lexer);
            continue;
        }

        // Operators & delimiters
        operators_and_delimiters(lexer);
    }
}


