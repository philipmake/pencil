#include "symtab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// create a symbol table, this is done per-file.
symtab_t* symtab_create(void)
{
    symtab_t* table = (symtab_t*)malloc(sizeof(symtab_t));
    if (table == NULL) return NULL;

    memset(table->scopes, 0, sizeof(table->scopes));
    table->current_depth = 0;
    table->current_scope = NULL;
    table->global_scope = NULL;

    return table;
}

void symtab_destroy(symtab_t* table)
{
    for (int i = 0; i < table->current_depth; i++)
    {
        scope_destroy(table->scopes[i]);
    }
    free(table);
}

// symbol operations
sym_entry_t** grow_array_sym(sym_entry_t** refs, int* count, sym_entry_t* ref)
{
    refs = realloc(refs, sizeof(sym_entry_t*) * (*count + 1));
    if (!refs)
    {
        printf("Out of memory\n");
        exit(1);
    }
    refs[*count] = ref;
    (*count)++;
    return refs;
}

sym_entry_t* symtab_insert(symtab_t* table, sym_entry_t* symbol)
{
    if (table == NULL)
    {
        printf("Symbol table doesn't exist!!!\n");
        return NULL;
    }
    
    table->current_scope->symbols = grow_array_sym(
        table->current_scope->symbols, 
        &(table->current_scope->symbol_count), 
        symbol
    );
    
    return symbol;
}

sym_entry_t* symtab_lookup(symtab_t* table, const char* name)
{
    if (table == NULL || table->current_scope == NULL) return NULL;

    scope_t* scope = table->current_scope;

    while (scope != NULL)
    {
        for (int i = 0; i < scope->symbol_count; i++)
        {
            if (strcmp(scope->symbols[i]->name, name) == 0)
            {
                return scope->symbols[i];
            }
        }

        scope = scope->parent;
    }

    return NULL;  // meaning symbol doesn't exist yet
}

sym_entry_t* symtab_lookup_current_scope(symtab_t* table, const char* name)
{
    if (table == NULL) return NULL;

    for (int i = 0; i < table->current_scope->symbol_count; i++) 
    {
        if (strcmp(table->current_scope->symbols[i]->name, name) == 0)
        {
            return table->current_scope->symbols[i];
        }
    }
    return NULL;  // meaning symbol doesn't exist yet   
}

int symtab_remove(symtab_t* table, const char* name)
{
    if (table == NULL || table->current_scope == NULL) return 0;
    
    scope_t* scope = table->current_scope;

    for (int i = 0; i < scope->symbol_count; i++) 
    {
        if (strcmp(scope->symbols[i]->name, name) == 0)
        {
            sym_destroy(scope->symbols[i]);

            for (int j = i; j < scope->symbol_count - 1; j++)
            {
                scope->symbols[j] = scope->symbols[j + 1];
            }

            scope->symbol_count--;
            return 1;  // found and removed
        }
    }
    return 0;  // not found and or removed
}

// Reference tracking
void symtab_add_reference(sym_entry_t* symbol, int line, int is_write)
{
    reference_t* ref = (reference_t*)malloc(sizeof(reference_t));

    ref->line = line;
    ref->is_write = is_write;

    reference_t** refs = symbol->references;
    symbol->references = grow_array(refs, &(symbol->ref_count), ref);
}

void reference_destroy(reference_t* ref)
{
    // ensure to reduce ref_count after using this function
    free(ref);
}

// symbol entry operations
sym_entry_t* sym_entry_create(const char* name, symbol_t type, 
                              datatype_t data_type, int line)
{
    sym_entry_t* sym = (sym_entry_t*)malloc(sizeof(sym_entry_t));

    if (sym == NULL) return NULL;

    strncpy(sym->name, name, MAX_NAME_LEN - 1);
    sym->name[MAX_NAME_LEN - 1] = '\0';
    sym->symbol_type = type;
    sym->type = data_type;
    sym->address = 0;
    sym->line = line;
    sym->level = 0;
    sym->scope = NULL;

    sym->references = NULL;
    sym->ref_count = 0;

    return sym;
}


void sym_destroy(sym_entry_t* entry)
{
    for (int i = 0; i < entry->ref_count; i++)
    {
        free(entry->references[i]);
    }
    free(entry);
}

void symtab_print(symtab_t* table);
void symtab_print_scope(scope_t* scope);
int symtab_check_redeclaration(symtab_t* table, const char* name);


reference_t** grow_array(reference_t** refs, int* count, reference_t* ref)
{
    refs = realloc(refs, sizeof(reference_t*) * (*count + 1));
    if (!refs)
    {
        printf("Out of memory\n");
        exit(1);
    }
    refs[*count] = ref;
    (*count)++;
    return refs;
}


