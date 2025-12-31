#include "symtab.h"
#include "scope.h"

#include <stdlib.h>
#include <stdio.h>

scope_t* create_global_scope()
{
    scope_t* sc = (scope_t*)malloc(sizeof(scope_t));

    if (sc == NULL)
    {
        printf("Error creating scope\n");
        return NULL;
    }
    sc->level = 0;
    sc->symbols = NULL;
    sc->symbol_count = 0;
    sc->parent = NULL;
    sc->flags = 0;

    return sc;
}

void symtab_enter_scope(symtab_t* table)
{
    int new_depth = table->current_depth + 1;
    scope_t* new_scope = scope_create(new_depth, table->current_scope);

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

scope_t* scope_create(int level, scope_t* parent)
{
    scope_t* scope = (scope_t*)malloc(sizeof(scope_t));
    if (scope == NULL)
    {
        printf("Error creating scope\n");
        return NULL;
    }

    scope->level = level;
    scope->parent = parent;
    scope->symbols = NULL;
    scope->symbol_count = 0;
    scope->flags = 0;

    return scope;
}

void scope_destroy(scope_t* scope)
{
    if(scope == NULL) return;

    for (int i = 0; i < scope->symbol_count; i++)
    {
        sym_entry_t* sym = scope->symbols[i];
        sym_destroy(sym);
    }

    free(scope->symbols);
    free(scope);
}



