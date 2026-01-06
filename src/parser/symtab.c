#include "symtab.h"
#include "scope.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// create a symbol table, this is done per-file.
symtab_t* symtab_create(void)
{
    printf("DEBUG: symtab_create - START\n");
    fflush(stdout);
    
    symtab_t* table = malloc(sizeof(symtab_t));
    if (!table) {
        fprintf(stderr, "Error: Failed to allocate symbol table\n");
        return NULL;
    }
    
    printf("DEBUG: symtab_create - table allocated at %p\n", (void*)table);
    fflush(stdout);
    
    // Initialize all scope pointers to NULL
    for (int i = 0; i < MAX_DEPTH; i++) {
        table->scopes[i] = NULL;
    }
    
    printf("DEBUG: symtab_create - calling create_global_scope\n");
    fflush(stdout);
    
    // Create global scope
    table->global_scope = create_global_scope();
    if (!table->global_scope) {
        fprintf(stderr, "Error: Failed to create global scope\n");
        free(table);
        return NULL;
    }
    
    printf("DEBUG: symtab_create - global_scope=%p\n", (void*)table->global_scope);
    fflush(stdout);
    
    table->scopes[0] = table->global_scope;
    table->current_scope = table->global_scope;
    table->current_depth = 0;
    
    printf("DEBUG: symtab_create - COMPLETE\n");
    printf("       table=%p\n", (void*)table);
    printf("       global_scope=%p\n", (void*)table->global_scope);
    printf("       current_scope=%p\n", (void*)table->current_scope);
    printf("       current_depth=%d\n", table->current_depth);
    printf("       scopes[0]=%p\n", (void*)table->scopes[0]);
    fflush(stdout);
    
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
    printf("DEBUG: Growing symbol array from %d to %d\n", *count, *count + 1);
    
    if (!count) {
        fprintf(stderr, "ERROR: grow_array_sym - count is NULL\n");
        return refs;
    }
    
    if (!ref) {
        fprintf(stderr, "ERROR: grow_array_sym - ref is NULL\n");
        return refs;
    }
    
    int new_count = *count + 1;
    
    // Handle NULL (first allocation)
    sym_entry_t** new_refs;
    if (refs == NULL) {
        new_refs = malloc(sizeof(sym_entry_t*) * new_count);
    } else {
        new_refs = realloc(refs, sizeof(sym_entry_t*) * new_count);
    }
    
    if (!new_refs) {
        fprintf(stderr, "Error: Failed to grow symbol array\n");
        return refs;
    }
    
    new_refs[*count] = ref;
    *count = new_count;
    
    printf("DEBUG: Symbol array grown successfully to %d elements\n", new_count);
    
    return new_refs;
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
    printf("DEBUG: symtab_lookup_current_scope called with name='%s'\n", 
           name ? name : "NULL");
    
    if (!table) {
        fprintf(stderr, "ERROR: table is NULL\n");
        fflush(stderr);
        return NULL;
    }
    printf("DEBUG: table=%p\n", (void*)table);
    
    if (!name) {
        fprintf(stderr, "ERROR: name is NULL\n");
        fflush(stderr);
        return NULL;
    }
    
    printf("DEBUG: table->current_scope=%p\n", (void*)table->current_scope);
    printf("DEBUG: table->current_depth=%d\n", table->current_depth);
    
    if (!table->current_scope) {
        fprintf(stderr, "ERROR: current_scope is NULL!\n");
        fprintf(stderr, "       Checking scopes array...\n");
        for (int i = 0; i < MAX_DEPTH; i++) {
            fprintf(stderr, "       scopes[%d]=%p\n", i, (void*)table->scopes[i]);
            if (table->scopes[i]) break;
        }
        fflush(stderr);
        return NULL;
    }
    
    scope_t* scope = table->current_scope;
    printf("DEBUG: scope->symbol_count=%d\n", scope->symbol_count);
    printf("DEBUG: scope->symbols=%p\n", (void*)scope->symbols);
    
    if (scope->symbols == NULL && scope->symbol_count == 0) {
        printf("DEBUG: Scope is empty (no symbols yet)\n");
        return NULL;
    }
    
    if (scope->symbols == NULL) {
        fprintf(stderr, "ERROR: symbols array is NULL but count=%d\n", scope->symbol_count);
        fflush(stderr);
        return NULL;
    }
    
    for (int i = 0; i < scope->symbol_count; i++) {
        printf("DEBUG: Checking symbol %d/%d\n", i, scope->symbol_count);
        if (scope->symbols[i]) {
            printf("DEBUG: Symbol %d name='%s'\n", i, scope->symbols[i]->name);
            if (strcmp(scope->symbols[i]->name, name) == 0) {
                printf("DEBUG: Found '%s' in current scope\n", name);
                return scope->symbols[i];
            }
        } else {
            printf("DEBUG: Symbol %d is NULL\n", i);
        }
    }
    
    printf("DEBUG: '%s' not found in current scope\n", name);
    return NULL;
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

sym_entry_t* sym_create(const char* name, symbol_t type, datatype_t data_type, int line)
{
    if (!name) return NULL;
    
    sym_entry_t* entry = malloc(sizeof(sym_entry_t));
    if (!entry) 
    {
        fprintf(stderr, "Error: Failed to allocate symbol entry\n");
        return NULL;
    }
    
    // Copy name safely
    strncpy(entry->name, name, MAX_NAME_LEN - 1);
    entry->name[MAX_NAME_LEN - 1] = '\0';
    
    entry->symbol_type = type;
    entry->type = data_type;
    entry->address = 0;
    entry->line = line;
    entry->level = 0;
    entry->scope = NULL;
    entry->references = NULL;
    entry->ref_count = 0;
    
    // Initialize union based on symbol type
    switch (type) 
    {
        case SYM_VARIABLE:
        case SYM_CONSTANT:
            entry->info.var.initial_value = NULL;
            entry->info.var.size = 0;
            entry->info.var.is_constant = (type == SYM_CONSTANT) ? 1 : 0;
            break;
            
        case SYM_FUNCTION:
            entry->info.func.params = NULL;
            entry->info.func.param_count = 0;
            entry->info.func.is_defined = 0;
            break;
            
        case SYM_PARAM:
            entry->info.param.position = 0;
            entry->info.param.offset = 0;
            break;
            
        case SYM_ARRAY:
            entry->info.array.dimensions = 0;
            entry->info.array.size = 0;
            break;
            
        case SYM_LABEL:
            entry->info.label.target_line = 0;
            entry->info.label.line_used = 0;
            break;
            
        default:
            break;
    }
    
    return entry;
}

void sym_destroy(sym_entry_t* entry)
{
    for (int i = 0; i < entry->ref_count; i++)
    {
        free(entry->references[i]);
    }
    free(entry);
}

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



const char* symbol_type_to_string(symbol_t type)
{
    switch (type) {
        case SYM_FILE:      return "FILE";
        case SYM_FUNCTION:  return "FUNCTION";
        case SYM_CONSTANT:  return "CONSTANT";
        case SYM_VARIABLE:  return "VARIABLE";
        case SYM_PARAM:     return "PARAMETER";
        case SYM_ARRAY:     return "ARRAY";
        case SYM_STRUCT:    return "STRUCT";
        case SYM_ENUM:      return "ENUM";
        case SYM_LABEL:     return "LABEL";
        default:            return "UNKNOWN";
    }
}

const char* datatype_to_string(datatype_t type)
{
    switch (type) {
        case TYPE_VOID:     return "void";
        case TYPE_INT:      return "int";
        case TYPE_FLOAT:    return "float";
        case TYPE_DOUBLE:   return "double";
        case TYPE_CHAR:     return "char";
        case TYPE_BOOL:     return "bool";
        case TYPE_STRING:   return "string";
        case TYPE_ARRAY:    return "array";
        case TYPE_POINTER:  return "pointer";
        case TYPE_STRUCT:   return "struct";
        case TYPE_ENUM:     return "enum";
        case TYPE_UNKNOWN:  return "unknown";
        default:            return "???";
    }
}

void symtab_print_scope(scope_t* scope)
{
    if (!scope) return;
    
    printf("\n  --- Scope Level %d (Symbols: %d) ---\n", scope->level, scope->symbol_count);
    
    if (scope->flags & FUNCTION) printf("  [FUNCTION SCOPE]\n");
    if (scope->flags & LOOP) printf("  [LOOP SCOPE]\n");
    
    for (int i = 0; i < scope->symbol_count; i++) {
        sym_entry_t* sym = scope->symbols[i];
        if (!sym) continue;
        
        printf("  %-20s | %-12s | %-10s | Line: %-4d | Refs: %d",
               sym->name,
               symbol_type_to_string(sym->symbol_type),
               datatype_to_string(sym->type),
               sym->line,
               sym->ref_count);
        
        // Print extra info based on symbol type
        switch (sym->symbol_type) {
            case SYM_FUNCTION:
                printf(" | Params: %d | Defined: %s",
                       sym->info.func.param_count,
                       sym->info.func.is_defined ? "yes" : "no");
                break;
                
            case SYM_ARRAY:
                printf(" | Dims: %d | Size: %d",
                       sym->info.array.dimensions,
                       sym->info.array.size);
                break;
                
            case SYM_PARAM:
                printf(" | Pos: %d | Offset: %d",
                       sym->info.param.position,
                       sym->info.param.offset);
                break;
                
            case SYM_VARIABLE:
            case SYM_CONSTANT:
                printf(" | Const: %s",
                       sym->info.var.is_constant ? "yes" : "no");
                break;
                
            default:
                break;
        }
        
        printf("\n");
        
        // Print references
        if (sym->ref_count > 0) {
            printf("    References: ");
            for (int j = 0; j < sym->ref_count; j++) {
                printf("L%d%s", 
                       sym->references[j]->line,
                       sym->references[j]->is_write ? "(W)" : "(R)");
                if (j < sym->ref_count - 1) printf(", ");
            }
            printf("\n");
        }
    }
}

void symtab_print(symtab_t* table)
{
    if (!table) {
        printf("Symbol table is NULL\n");
        return;
    }
    
    printf("\n");
    printf("================================================================================\n");
    printf("                           SYMBOL TABLE\n");
    printf("================================================================================\n");
    printf("Current Depth: %d\n", table->current_depth);
    
    // Print all scopes from global to current
    for (int i = 0; i <= table->current_depth; i++) {
        if (table->scopes[i]) {
            symtab_print_scope(table->scopes[i]);
        }
    }
    
    printf("\n");
    printf("================================================================================\n");
    printf("\n");
}

