#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Keyword list for recognizing keywords
const char *keywords[] = 
{
    "if", 
    "else", 
    "loop", 
    "for",  
    "match", 
    "import", 
    "struct", 
    "variant", 
    "enum",  
    "vec",  
    "byte",  
    "short",  
    "int", 
    "long", 
    "char",
    "str", 
    "float",  
    "double",  
    "var",
    "let",
    "fn",
    "main",
    "let",
    "return",
    "void"
};

int is_keyword(const char *str) 
{
    for (int i = 0; keywords[i] != NULL; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1; // True
        }
    }
    return 0; // False
}

TokenType keyword_to_token(const char *keyword)
{
    if (strcmp(keyword, "if") == 0) return IF;
    else if (strcmp(keyword, "else") == 0) return ELSE;
    else if (strcmp(keyword, "loop") == 0) return LOOP;
    else if (strcmp(keyword, "for") == 0) return FOR;
    else if (strcmp(keyword, "case") == 0) return CASE;
    else if (strcmp(keyword, "import") == 0) return IMPORT;
    else if (strcmp(keyword, "struct") == 0) return STRUCT;
    else if (strcmp(keyword, "enum") == 0) return ENUM;
    else if (strcmp(keyword, "break") == 0) return BREAK;
    else if (strcmp(keyword, "continue") == 0) return CONTINUE;
    else if (strcmp(keyword, "fn") == 0) return FN;
    else if (strcmp(keyword, "main") == 0) return MAIN;
    else if (strcmp(keyword, "str") == 0) return TYPE;
    else if (strcmp(keyword, "char") == 0) return TYPE;
    else if (strcmp(keyword, "int") == 0) return TYPE;
    else if (strcmp(keyword, "long") == 0) return TYPE;
    else if (strcmp(keyword, "float") == 0) return TYPE;
    else if (strcmp(keyword, "let") == 0) return LET;
    else if (strcmp(keyword, "var") == 0) return VAR;
    else if (strcmp(keyword, "match") == 0) return MATCH;
    else if (strcmp(keyword, "union") == 0) return UNION;
    else if (strcmp(keyword, "return") == 0) return RETURN;
    else if (strcmp(keyword, "void") == 0) return VOID;
    else return UNKNOWN;
}


const char* tokentype_to_string(TokenType type) 
{
    switch (type) {
        case IF: return "IF";
        case ELSE: return "ELSE";
        case FOR: return "FOR";
        case LOOP: return "LOOP";
        case BREAK: return "BREAK";
        case CONTINUE: return "CONTINUE";
        case MATCH: return "MATCH";

        case FN: return "FN";
        case RETURN: return "RETURN";
        case VOID: return "VOID";
        case MAIN: return "MAIN";

        case STRUCT: return "STRUCT";
        case UNION: return "UNION";
        case ENUM: return "ENUM";
        case TYPE: return "TYPE";
        case IMPORT: return "IMPORT";

        case INT_LITERAL: return "INT_LITERAL";
        case FLOAT_LITERAL: return "FLOAT_LITERAL";
        case CHAR_LITERAL: return "CHAR_LITERAL";
        case BOOL_LITERAL: return "BOOL_LITERAL";
        case HEX_LITERAL: return "HEX_LITREAL";
        case OCTAL_LITERAL: return "OCTAL_LITREAL";
        case BINARY_LITERAL: return "BINARY_LITREAL";
        case STRING_LITERAL: return "STRING_LITERAL";

        case WHITESPACE: return "WHITESPACE";
        case NEWLINE: return "NEWLINE";

        case IDENTIFIER: return "IDENTIFIER";
        case KEYWORD: return "KEYWORD";
        case VAR: return "VAR";
        case LET: return "LET";

        case OPEN_BRACKET: return "OPEN_BRACKET";
        case CLOSE_BRACKET: return "CLOSE_BRACKET";
        case OPEN_CURLY: return "OPEN_CURLY";
        case CLOSE_CURLY: return "CLOSE_CURLY";
        case OPEN_PAREN: return "OPEN_PAREN";
        case CLOSE_PAREN: return "CLOSE_PAREN";
        case SEMICOLON: return "SEMICOLON";
        case COLON: return "COLON";
        case COMMA: return "COMMA";
        case QUOTE: return "QUOTE";
        case DOUBLE_QUOTE: return "DOUBLE_QUOTE";
        case DOT: return "DOT";
        case ELLIPSIS: return "ELLIPSIS";
        case ARROW: return "ARROW";
        case UNDERSCORE: return "UNDERSCORE";

        case TOKEN_EOF: return "EOF";

        case PLUS: return "PLUS";
        case PLUS_PLUS: return "PLUS_PLUS";
        case MINUS: return "MINUS";
        case MINUS_MINUS: return "MINUS_MINUS";
        case STAR: return "STAR";
        case SLASH: return "SLASH";
        case PERCENT: return "PERCENT";

        case EQUAL: return "EQUAL";
        case NOT_EQUAL: return "NOT_EQUAL";
        case LESS: return "LESS";
        case LESS_EQUAL: return "LESS_EQUAL";
        case GREATER: return "GREATER";
        case GREATER_EQUAL: return "GREATER_EQUAL";

        case AND: return "AND";
        case OR: return "OR";
        case NOT: return "NOT";

        case BITWISE_AND: return "BITWISE_AND";
        case BITWISE_OR: return "BITWISE_OR";
        case BITWISE_XOR: return "BITWISE_XOR";
        case BITWISE_NOT: return "BITWISE_NOT";
        case LSHIFT: return "LSHIFT";
        case RSHIFT: return "RSHIFT";

        case ASSIGN: return "ASSIGN";
        case PLUS_ASSIGN: return "PLUS_ASSIGN";
        case MINUS_ASSIGN: return "MINUS_ASSIGN";
        case STAR_ASSIGN: return "STAR_ASSIGN";
        case SLASH_ASSIGN: return "SLASH_ASSIGN";
        case PERCENT_ASSIGN: return "PERCENT_ASSIGN";

        default: return "UNKNOWN";
    }
}

// Create a new token array
TokenArray* create_array() 
{
    TokenArray* arr = malloc(sizeof(TokenArray));
    if (!arr) {
        printf("Error: Failed to allocate memory for token array\n");
        exit(1);
    }
    
    arr->tokens = malloc(sizeof(Token*) * 16);
    if (!arr->tokens) {
        printf("Error: Failed to allocate memory for tokens\n");
        free(arr);
        exit(1);
    }
    
    arr->token_count = 0;
    arr->capacity = 16;
    return arr;
}


// Global token array instance
TokenArray* global_array = 0;
void init_global_array() 
{
    if (!global_array) {
        global_array = create_array();
    }
}

extern char* filename;

// Create new token
Token* create_token(TokenType type, char* lexeme, int length, int line, int column) 
{
    Token* token = malloc(sizeof(Token));
    if (!token) 
    {
        printf("Error: Failed to allocate memory for token\n");
        exit(1);
    }
    
    token->type = type;
    token->location.filename = filename;
    token->location.line = line;
    token->location.column = column;
    
    // Allocate and copy lexeme
    if (lexeme && length > 0)
    {
        token->lexeme = malloc(length + 1);
        if (!token->lexeme) 
        {
            printf("Error: Failed to allocate memory for lexeme\n");
            free(token);
            exit(1);
        }
        strncpy(token->lexeme, lexeme, length);
        token->lexeme[length] = '\0';
    } else 
    {
        token->lexeme = malloc(1);
        token->lexeme[0] = '\0';
    }
    
    return token;
}


// Add a token to the global_array
// first check for space and resize if needed
// then create_token ...
// add new token to token list
void add_token(TokenType type, char* lexeme, int length, int line, int column) 
{
    if (!global_array) 
    {
        init_global_array();
        printf("Error: Token array doesn't exist\n"); return;
    }

    if (global_array->token_count >= global_array->capacity)
    {
        int new_cap = global_array->capacity * 2;
        Token** new_tokens_list = realloc(global_array->tokens, sizeof(Token*) * new_cap);
        if (!new_tokens_list)
        {
            printf("Error: Token array full. Failed to resize.\n"); exit(1);
        }
        global_array->tokens = new_tokens_list;
        global_array->capacity = new_cap;
        printf("Token array resized to capacity: %d\n", new_cap);
    }

    Token* tk = create_token(type, lexeme, length, line, column);
    global_array->tokens[global_array->token_count] = tk;
    global_array->token_count++;
}

// Clean up functions
void free_token(Token* token) 
{
    if (token) 
    {
        if (token->lexeme) 
        {
            free(token->lexeme);
        }
        free(token);
    }
}

void free_array(TokenArray* arr) 
{
    if (arr) 
    {
        for (int i = 0; i < arr->token_count; i++) 
        {
            free_token(arr->tokens[i]);
        }
        free(arr->tokens);
        free(arr);
    }
}

// --------------- Print functions -------------------------------
void print_token(Token* token) 
{
    if (token == 0) 
    { 
        printf("NULL TOKEN\n");
        return;
    }
    
    printf("%-20s | %-20s | Line: %d, Col: %d\n", 
           tokentype_to_string(token->type),
           token->lexeme ? token->lexeme : "(null)",
           token->location.line,
           token->location.column);
}


// print all tokens from TokenArray object
void print_all_tokens(TokenArray* arr) 
{
    if (!arr) 
    {
        printf("Token array is NULL\n");
        return;
    }
    
    printf("=== Token Table ===\n");
    printf("%-20s | %-20s | Position\n", "Type", "Lexeme");
    printf("---------------------------------------------------------------\n");
    
    for (int i = 0; i < arr->token_count; i++) 
    {
        if (arr->tokens[i] != 0) 
        {
            print_token(arr->tokens[i]);
        }
    }
    
    printf("\nTotal tokens: %d (Capacity: %d)\n", arr->token_count, arr->capacity);
}


// print tokens as they would appear in source
void print_tokens_as_source(TokenArray* arr) 
{
    if (!arr) 
    {
        printf("Token array is NULL\n");
        return;
    }
    
    printf("=== source reconstruction ===\n");
    for (int i = 0; i < arr->token_count; i++) 
    {
        if (arr->tokens[i] != 0 && arr->tokens[i]->lexeme != 0) 
        {
            printf("%s", arr->tokens[i]->lexeme);
        }
    }
    printf("\n");
}

// Print token statistics
void print_stats(TokenArray* arr) 
{
    if (!arr) 
    {
        printf("Token array is NULL\n");
        return;
    }
    
    int token_counts[50] = {0}; // Adjust size based on your token types
    
    for (int i = 0; i < arr->token_count; i++) 
    {
        if (arr->tokens[i] != 0 && arr->tokens[i]->type < 50) 
        {
            token_counts[arr->tokens[i]->type]++;
        }
    }
    
    printf("=== STATS ===\n");
    for (int i = 0; i < 50; i++) 
    {
        if (token_counts[i] > 0) 
        {
            printf("%-20s: %d\n", tokentype_to_string((TokenType)i), token_counts[i]);
        }
    }
    printf("Total: %d tokens\n", arr->token_count);
}



// Convenience functions for global array
void print_all_tokens_global() 
{
    print_all_tokens(global_array);
}

void print_tokens_as_source_global() 
{
    print_tokens_as_source(global_array);
}

void print_stats_global() 
{
    print_stats(global_array);
}