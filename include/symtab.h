#ifndef SYMTAB_H_
#define SYMTAB_H_

#include <stdlib.h>
#include "scope.h"

#define MAX_NAME_LEN    256
#define MAX_DEPTH       64
#define MAX_SYMBOLS     1024


typedef enum 
{
    SYM_FILE,
    SYM_FUNCTION,
    SYM_CONSTANT,
    SYM_VARIABLE,
    SYM_PARAM,
    SYM_ARRAY,
    SYM_STRUCT,
    SYM_ENUM,
    SYM_LABEL
} symbol_t;


typedef enum 
{
    TYPE_VOID,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_POINTER,
    TYPE_STRUCT,
    TYPE_ENUM,
    TYPE_UNKNOWN
} datatype_t;


typedef struct
{
    void* initial_value;
    int size;
    int is_constant;
} varSym;

typedef struct
{
    int dimensions;
    int size;
} arraySym;

typedef struct
{
    sym_entry_t** params;
    int param_count;
    int is_defined;
} funcSym;

typedef struct
{
    int position;
    int offset; // stack offset
} paramSym;

typedef struct
{
    int target_line;
    int line_used;
} lbSym;

// Forward declarations
typedef struct reference_t reference_t;
typedef struct sym_entry_t sym_entry_t;
typedef struct scope_t scope_t;

// Reference structure - tracks where a symbol is used
struct reference_t {
    int line;
    int is_write;           // 1 if this is a write/assignment, 0 if read
};

// Symbol entry structure
struct sym_entry_t {
    char name[MAX_NAME_LEN];
    symbol_t symbol_type;
    datatype_t type;
    unsigned long address;
    int line;
    int level;      // i.e. scope
    scope_t* scope; // pointer to scope

    reference_t** references; // linked list of all references
    int ref_count;          // number of references to this symbol
    
    union 
    {
        varSym var;
        paramSym param;
        funcSym func;
        lbSym label;
        arraySym array;
    } info;
};


// Symbol table structure
struct symtab_t{
    scope_t* scopes[MAX_DEPTH]; // stack of scopes
    int current_depth;
    scope_t* current_scope;
    scope_t* global_scope;
};


// Function prototypes
symtab_t* symtab_create(void);
void symtab_destroy(symtab_t* table);

// Symbol operations
sym_entry_t* symtab_insert(symtab_t* table, sym_entry_t* symbol);

sym_entry_t* symtab_lookup(symtab_t* table, const char* name);
sym_entry_t* symtab_lookup_current_scope(symtab_t* table, const char* name);
int symtab_remove(symtab_t* table, const char* name);

// Reference tracking
void symtab_add_reference(sym_entry_t* symbol, int line, int is_write);
void reference_destroy(reference_t* ref);

// Symbol entry operations
sym_entry_t* sym_create(const char* name, symbol_t type, datatype_t data_type, int line);
void sym_destroy(sym_entry_t* entry);

// Utility functions
void symtab_print(symtab_t* table);
void symtab_print_scope(scope_t* scope);
int symtab_check_redeclaration(symtab_t* table, const char* name);

reference_t** grow_array(reference_t** refs, int* count, reference_t* ref);
sym_entry_t** grow_array_sym(sym_entry_t** refs, int* count, sym_entry_t* ref);

#endif