#ifndef SCOPE_H_
#define SCOPE_H_


typedef struct sym_entry_t sym_entry_t;
typedef struct scope_t scope_t;

#define GLOBAL      0x001
#define LOCAL       0x002
#define FUNCTION    0x004
#define LOOP        0x008

// Scope structure - represents a lexical scope
struct scope_t {
    int level;              // nesting level (0 = global)
    sym_entry_t** symbols;  // hash table or array of symbols
    int symbol_count;       // number of symbols in this scope
    scope_t* parent;        // pointer to enclosing scope
    int flags;              // check if scope is in a function or a loop
};


// Scope management
scope_t* create_global_scope();

typedef struct symtab_t symtab_t;
void symtab_enter_scope(symtab_t* table);
void symtab_exit_scope(symtab_t* table);
scope_t* scope_create(int level, scope_t* parent);
void scope_destroy(scope_t* scope);

#endif