#ifndef AST_H_ 
#define AST_H_

#include "token.h"
#include <stdlib.h>

typedef struct ASTNode ASTNode;

// ASTNode types
typedef enum 
{
    AST_BLOCK,
    AST_PROGRAM,
    AST_STMT,
    AST_LITERAL,
    AST_IDENTIFIER,
    AST_UNARY,
    AST_BINARY,
    AST_ASSIGN,
    AST_INDEX,
    AST_FN_CALL,
    AST_RANGE,
    AST_VAR_DECL,
    AST_CONST_DECL,
    AST_ARRAY_DECL,
    AST_FN_DECL,
    AST_PARAM,
    AST_STRUCT,
    AST_ENUM,
    AST_VECTOR,
    AST_FIELD,
    AST_UNION,
    AST_IF,
    AST_MATCH,
    AST_MATCH_CASE,
    AST_FOR,
    AST_LOOP,
    AST_RETURN
} ASTNodeType;

// Expression structures
typedef struct 
{
    Token* op;
    ASTNode* operand;
} UnaryExpr;

typedef struct 
{
    ASTNode* left;
    Token* op;
    ASTNode* right;
} BinaryExpr;

typedef struct 
{
    Token* name;
    Token* op;
    ASTNode* value;
} AssignExpr;

typedef struct 
{
    ASTNode* base;
    ASTNode* index;
} Index;

typedef struct 
{
    ASTNode* callee;    // an identifier
    ASTNode** args;     // dynamic array
    int arg_count;
} FnCall;

typedef struct 
{
    ASTNode* start;
    ASTNode* end;
    ASTNode* inc_end;   // inclusive end e.g [<8] stops at 7
    ASTNode* step;  // this field describe the rate of increase or decrease of the range value
} Range;

// Identifier token
typedef struct
{                             
    char* name; // array of 64 characters max
} Identifier;

// Literal token char, int, float, string etc.
typedef struct
{
    char* value;
} Literal;

// variable and constant declarations
typedef struct
{
    ASTNodeType type;
    Token* data_type; // int, float or char??
    ASTNode* ident;
    ASTNode* value;
} Decl;

// function declaration
typedef struct
{
    ASTNode* ident;
    ASTNode** params;
    size_t params_count;
    Token* return_type;
    ASTNode* block;
    ASTNode* return_stmt;  // work with return statements
}  FuncDecl;

typedef struct
{
    ASTNode* expr;
} ReturnStmt;

typedef struct
{
    ASTNode* ident;
    Token* type;
} Param;

typedef struct
{
    ASTNode* ident;
    Token* type;
    ASTNode* range;
    ASTNode** literals;
    size_t literal_count;
} Array;

typedef struct 
{
    ASTNode** statements;  // array of statements inside the block
    size_t count;          // how many statements
} Block;

typedef struct 
{
    ASTNode* condition;       // may be NULL for plain else
    ASTNode** then_branch;    // statements inside this block
    size_t then_count;
    ASTNode* else_branch;     // points to another IfStmt (for else-if) or a block (for else)
} IfStmt;

typedef struct
{
    ASTNode* pattern;
    ASTNode** match_cases;
    size_t case_count;
    ASTNode* def_case;
} MatchStmt;

typedef struct
{
    ASTNode* expr;
    ASTNode* stmt;
} MatchCase;

typedef struct
{
    ASTNode* expr;
    ASTNode* block;
} ForLoop;

typedef struct
{
    ASTNode* block;
} Loop;

typedef struct
{
    ASTNode* enum_name;
    ASTNode** enum_values;
    size_t enum_count;
} Enum;

typedef struct
{
    ASTNode* ident;
    Token* type;
} StructField;

typedef struct
{
    ASTNode* name;
    ASTNode** fields;
    size_t fields_count;
} Struct;

typedef struct
{
    ASTNode* name;
    ASTNode** fields;
    size_t fields_count;
} Union;

typedef struct
{
    ASTNode** statements;
    int stmt_count;
} Program;


struct ASTNode
{
    ASTNodeType type;
    SourceLocation location;
    union
    {
        Identifier ident;
        Literal literal;
        AssignExpr assign;
        BinaryExpr binary;
        UnaryExpr unary;
        Index idx;
        FnCall call;
        Range rng;
        Decl declaration;
        Program program;
        Array arr;
        IfStmt ifstmt;
        Block block;
        MatchStmt matchstmt;
        MatchCase matchcase;
        FuncDecl func;
        Param param;
        StructField field;
        ForLoop forloop;
        Loop loop;
        Enum enumType;
        Struct structType;
        Union unionType;
        ReturnStmt return_stmt;
    } as;
};


// Simple arena-like allocator
void* parser_alloc(size_t size);

char* my_strdup(const char* s) ;

ASTNode* ast_new_literal(Token* t);
ASTNode* ast_new_identifier(Token* t);
ASTNode* ast_new_unary(Token* op, ASTNode* operand);
ASTNode* ast_new_binary(ASTNode* left, Token* op, ASTNode* right);
ASTNode* ast_new_assign(Token* name, Token* op, ASTNode* value);
ASTNode* ast_new_index(ASTNode* base, ASTNode* index);
ASTNode* ast_new_call(ASTNode* callee, ASTNode** args, int arg_count);
ASTNode* ast_new_range(ASTNode* start, ASTNode* end, ASTNode* inc_end, ASTNode* step);

ASTNode* ast_var_decl(ASTNode* ident, Token* data_type, ASTNode* value);
ASTNode* ast_const_decl(ASTNode* ident, Token* data_type, ASTNode* value);
ASTNode* ast_new_array(ASTNode* ident, Token* type, ASTNode* range, ASTNode** literals, size_t count);
ASTNode* ast_param(ASTNode* ident, Token* type);
ASTNode* ast_fn_decl(ASTNode* ident, 
                     ASTNode** params, 
                     size_t params_count, 
                     Token* return_type, 
                     ASTNode* block);

ASTNode* ast_block(ASTNode** statements, size_t count);
ASTNode* ast_return_stmt(ASTNode* expr);
ASTNode* ast_if(ASTNode* condition,
                ASTNode** then_branch, size_t then_count,
                ASTNode* else_branch);

ASTNode* ast_new_match_case(ASTNode* expr, ASTNode* result);
ASTNode* ast_new_match_stmt(ASTNode* pattern, ASTNode** match_cases, size_t case_count, ASTNode* def_case);
ASTNode* ast_for_loop(ASTNode* expr, ASTNode* block);
ASTNode* ast_loop(ASTNode* block);

ASTNode* ast_field(ASTNode* ident, Token* data_type);
ASTNode* ast_enum(ASTNode* enum_name, ASTNode** enum_values, size_t enum_count);
ASTNode* ast_struct(ASTNode* name, ASTNode** fields, size_t fields_count);
ASTNode* ast_union(ASTNode* name, ASTNode** fields, size_t fields_count);

ASTNode* ast_stmt();
ASTNode* ast_program(void);
void add_stmt(ASTNode* program, ASTNode* stmt);

#endif