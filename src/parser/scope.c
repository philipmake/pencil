#include "symtab.h"
#include "scope.h"

#include <stdlib.h>
#include <stdio.h>

scope_t* scope_create(int level, scope_t* parent)
{
    printf("DEBUG: Creating scope at level %d\n", level);
    
    scope_t* scope = malloc(sizeof(scope_t));
    if (!scope) {
        fprintf(stderr, "Error: Failed to allocate scope\n");
        return NULL;
    }
    
    scope->level = level;
    scope->symbols = NULL;  // Start with NULL, will allocate on first insert
    scope->symbol_count = 0;
    scope->parent = parent;
    scope->flags = 0;
    scope->children = NULL;
    scope->children_cnt = 0;

    // Inherit certain flags from parent
    if (parent) {
        if (parent->flags & FUNCTION) {
            scope->flags |= FUNCTION;
        }
        if (parent->flags & LOOP) {
            scope->flags |= LOOP;
        }
    }
    
    printf("DEBUG: Scope created successfully at level %d\n", level);
    
    return scope;
}


scope_t* create_global_scope()
{
    printf("\n Global scope\n");
    return scope_create(0, NULL);
}

void symtab_enter_scope(symtab_t* table)
{
    int new_depth = table->current_depth + 1;
    scope_t* new_scope = scope_create(new_depth, table->current_scope);

    if (table->current_scope != NULL)
    {
        scope_t* parent = table->current_scope;
        parent->children = realloc(parent->children, 
                                   sizeof(scope_t*) * (parent->children_cnt + 1));
        parent->children[parent->children_cnt] = new_scope;
        parent->children_cnt++;
    }

    table->scopes[new_depth] = new_scope;
    table->current_scope = new_scope;
    table->current_depth = new_depth;
}

void symtab_exit_scope(symtab_t* table)
{
    if (table->current_depth == 0) return;

    scope_t* prev = table->current_scope;

    // could scope check for unused variables
    // before exit

    table->current_scope = prev->parent;
    table->current_depth--;
}

void scope_destroy(scope_t* scope)
{
    if(scope == NULL) return;

    for (int i = 0; i < scope->symbol_count; i++)
    {
        sym_entry_t* sym = scope->symbols[i];
        sym_destroy(sym);
    }

    if (scope->children) 
    {
        free(scope->children);
    }

    free(scope->symbols);
    free(scope);
}



