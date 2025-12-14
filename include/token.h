#ifndef H_
#define H_

extern const char *keywords[];
#define KEYWORDS 22

// Tokentypes
typedef enum 
{
    TOKEN_EOF,
    TOKEN_ERROR,
    IF,
    ELSE,
    MATCH,
    FOR,
    LOOP,
    BREAK,
    CONTINUE,
    CASE,
    FN,
    VOID,
    RETURN,
    MAIN,
    STRUCT,
    UNION,
    ENUM,
    TYPE,
    LET,
    VAR,
    IMPORT,

    INT_LITERAL,
    FLOAT_LITERAL,
    CHAR_LITERAL,
    BOOL_LITERAL,
    HEX_LITERAL,
    OCTAL_LITERAL,
    BINARY_LITERAL,
    STRING_LITERAL,
    
    WHITESPACE,
    NEWLINE,
    
    IDENTIFIER,
    KEYWORD,
    
    OPEN_BRACKET, // [
    CLOSE_BRACKET, // ]
    OPEN_CURLY,   // {
    CLOSE_CURLY,   // }
    OPEN_PAREN,   // (
    CLOSE_PAREN,   // )
    SEMICOLON,    
    COLON,    
    COMMA,    
    DOT,  
    ELLIPSIS, // ...
    QUOTE, // '  
    DOUBLE_QUOTE, // " 
    ARROW,    // =>
    UNDERSCORE,  // _

    PLUS,
    PLUS_PLUS,
    MINUS,
    MINUS_MINUS,
    STAR,
    STAR_STAR,
    SLASH,
    PERCENT,
    
    EQUAL,
    NOT_EQUAL,  
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    
    AND,
    OR,
    NOT,

    BITWISE_AND,
    BITWISE_OR,
    BITWISE_XOR,
    BITWISE_NOT,
    LSHIFT,
    RSHIFT,
    
    ASSIGN,
    PLUS_ASSIGN,
    AND_ASSIGN,
    MINUS_ASSIGN,
    STAR_ASSIGN,
    SLASH_ASSIGN,
    PERCENT_ASSIGN,
    
    UNKNOWN
} TokenType;

// location info of a token in the file. Used for the parser for ast nodes, see ast.h
typedef struct {
    const char* filename;
    int line;
    int column;
} SourceLocation;


// A token object
typedef struct 
{
    TokenType type;
    char *lexeme;
    SourceLocation location;
} Token;

// A collection of tokens created by the parser and used throughout the compilation process.
// DO NOT FREE until after complete compilation
typedef struct
{
    Token** tokens;
    int token_count;
    int capacity;
} TokenArray;

extern int  token_count;
extern TokenArray* global_array;                    // global_array is a TokenArray object

const char* tokentype_to_string(TokenType type);    // returns the string equivalent of a tokentype
TokenType keyword_to_token(const char *keyword);    // keywords.c -> returns the tokentype of a keyword ?? not sure how useful this is
int is_keyword(const char* str);

// Token Array Management
TokenArray* create_array();
void init_global_array();
void free_array(TokenArray* arr);


// Token Creation & Addition
Token* create_token(TokenType type, char* lexeme, int length, int line, int column);
void add_token(TokenType type, char* lexeme, int length, int line, int column);
void free_token(Token* token);

// Printing Functions
void print_token(Token* token);
void print_all_tokens(TokenArray* arr);
void print_tokens_as_source(TokenArray* arr);
void print_stats(TokenArray* arr);

// Global Array Utilities
void print_all_tokens_global();
void print_tokens_as_source_global();
void print_stats_global();


#endif