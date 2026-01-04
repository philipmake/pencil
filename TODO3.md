Great question! This is a crucial part of type inference. You need to **recursively determine the type of any expression or literal** assigned to a variable. Let me break this down.

## Type Inference Strategy

The key insight is that you need a function that can determine the type of **any** AST node that represents a value:

```c
Type* infer_type_from_node(ASTNode* node)
{
    if (node == NULL) {
        return NULL;
    }
    
    switch (node->type) {
        case AST_LITERAL:
            return infer_type_from_literal(node);
            
        case AST_IDENTIFIER:
            return infer_type_from_identifier(node);
            
        case AST_BINARY:
            return infer_type_from_binary(node);
            
        case AST_UNARY:
            return infer_type_from_unary(node);
            
        case AST_FN_CALL:
            return infer_type_from_call(node);
            
        case AST_INDEX:
            return infer_type_from_index(node);
            
        case AST_RANGE:
            return TYPE_RANGE; // or however you represent ranges
            
        // Add other expression types...
        
        default:
            report_error(node->location, 
                "Cannot infer type from this construct");
            return TYPE_ERROR;
    }
}
```

## Detailed Examples

### 1. Inferring Type from Literals

Looking at your AST, `Literal` has a `char* value` field. You need to determine what type that literal represents:

```c
Type* infer_type_from_literal(ASTNode* node)
{
    // The literal node contains a string representation of the value
    // You need to look at the original token to know what kind it was
    // OR parse the string to determine its type
    
    char* value = node->as.literal.value;
    
    // Check the format of the literal string
    
    // Boolean literals
    if (strcmp(value, "true") == 0 || strcmp(value, "false") == 0) {
        return TYPE_BOOL;
    }
    
    // Character literal: single char in quotes
    if (value[0] == '\'' && value[strlen(value)-1] == '\'') {
        return TYPE_CHAR;
    }
    
    // String literal: enclosed in double quotes
    if (value[0] == '"' && value[strlen(value)-1] == '"') {
        // Return string type (you might represent this as TYPE_STRING 
        // or as an array of chars)
        return create_string_type();
    }
    
    // Floating point: contains a decimal point
    if (strchr(value, '.') != NULL) {
        // Could be f32 or f64 - might default to f64
        return TYPE_F64;
    }
    
    // Check for binary literal (0b...)
    if (strlen(value) > 2 && value[0] == '0' && value[1] == 'b') {
        // Determine size based on value or default to smallest that fits
        return infer_integer_type_from_value(value, 2); // base 2
    }
    
    // Check for hex literal (0x...)
    if (strlen(value) > 2 && value[0] == '0' && value[1] == 'x') {
        return infer_integer_type_from_value(value, 16); // base 16
    }
    
    // Check for octal literal (0o...)
    if (strlen(value) > 2 && value[0] == '0' && value[1] == 'o') {
        return infer_integer_type_from_value(value, 8); // base 8
    }
    
    // Regular decimal integer
    // Check for sign to determine if signed or unsigned needed
    if (value[0] == '-') {
        return infer_signed_integer_type_from_value(value);
    } else {
        // Could be unsigned or signed - might default to s32
        return infer_integer_type_from_value(value, 10);
    }
}

// Helper: determine smallest integer type that fits the value
Type* infer_integer_type_from_value(char* value_str, int base)
{
    // Parse the value
    long long value = strtoll(value_str, NULL, base);
    
    // Return smallest type that fits
    if (value >= 0 && value <= UINT8_MAX)
        return TYPE_U8;
    else if (value >= 0 && value <= UINT16_MAX)
        return TYPE_U16;
    else if (value >= 0 && value <= UINT32_MAX)
        return TYPE_U32;
    else if (value >= 0)
        return TYPE_U64;
    else
        return infer_signed_integer_type_from_value(value_str);
}

Type* infer_signed_integer_type_from_value(char* value_str)
{
    long long value = strtoll(value_str, NULL, 10);
    
    if (value >= INT8_MIN && value <= INT8_MAX)
        return TYPE_S8;
    else if (value >= INT16_MIN && value <= INT16_MAX)
        return TYPE_S16;
    else if (value >= INT32_MIN && value <= INT32_MAX)
        return TYPE_S32;
    else
        return TYPE_S64;
}
```

### 2. Inferring Type from Identifiers

When the initializer is another variable:

```c
Type* infer_type_from_identifier(ASTNode* node)
{
    char* var_name = node->as.ident.name;
    
    // Look up the variable in the symbol table
    Symbol* sym = symbol_table_lookup(var_name);
    
    if (sym == NULL) {
        report_error(node->location, 
            "Undefined variable '%s'", var_name);
        return TYPE_ERROR;
    }
    
    // Return the type of the referenced variable
    return sym->type;
}
```

### 3. Inferring Type from Binary Expressions

This is where it gets interesting - you need type promotion rules:

```c
Type* infer_type_from_binary(ASTNode* node)
{
    ASTNode* left = node->as.binary.left;
    ASTNode* right = node->as.binary.right;
    Token* op = node->as.binary.op;
    
    // Recursively infer types of operands
    Type* left_type = infer_type_from_node(left);
    Type* right_type = infer_type_from_node(right);
    
    if (left_type == TYPE_ERROR || right_type == TYPE_ERROR) {
        return TYPE_ERROR;
    }
    
    // Result type depends on operator
    switch (op->type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_STAR:
        case TOKEN_SLASH:
        case TOKEN_PERCENT:
        {
            // Arithmetic operators: result is "wider" type
            // Type promotion: u8 < u16 < u32 < u64
            //                 s8 < s16 < s32 < s64
            //                 integer < f32 < f64
            
            if (!is_numeric_type(left_type) || !is_numeric_type(right_type)) {
                report_error(node->location, 
                    "Arithmetic operator requires numeric types");
                return TYPE_ERROR;
            }
            
            return promote_types(left_type, right_type);
        }
        
        case TOKEN_EQUAL_EQUAL:
        case TOKEN_BANG_EQUAL:
        case TOKEN_LESS:
        case TOKEN_GREATER:
        case TOKEN_LESS_EQUAL:
        case TOKEN_GREATER_EQUAL:
        {
            // Comparison operators always return bool
            return TYPE_BOOL;
        }
        
        case TOKEN_AND_AND:
        case TOKEN_OR_OR:
        {
            // Logical operators: operands must be bool, result is bool
            if (left_type != TYPE_BOOL || right_type != TYPE_BOOL) {
                report_error(node->location,
                    "Logical operator requires boolean operands");
                return TYPE_ERROR;
            }
            return TYPE_BOOL;
        }
        
        case TOKEN_AMPERSAND:
        case TOKEN_PIPE:
        case TOKEN_CARET:
        case TOKEN_LSHIFT:
        case TOKEN_RSHIFT:
        {
            // Bitwise operators: result is promoted integer type
            if (!is_integer_type(left_type) || !is_integer_type(right_type)) {
                report_error(node->location,
                    "Bitwise operator requires integer types");
                return TYPE_ERROR;
            }
            return promote_types(left_type, right_type);
        }
        
        default:
            report_error(node->location, "Unknown binary operator");
            return TYPE_ERROR;
    }
}

// Type promotion rules
Type* promote_types(Type* t1, Type* t2)
{
    // If either is float, result is float
    if (t1 == TYPE_F64 || t2 == TYPE_F64)
        return TYPE_F64;
    if (t1 == TYPE_F32 || t2 == TYPE_F32)
        return TYPE_F32;
    
    // Both are integers - return wider type
    int rank1 = get_type_rank(t1);
    int rank2 = get_type_rank(t2);
    
    return (rank1 > rank2) ? t1 : t2;
}

int get_type_rank(Type* t)
{
    // Higher rank = wider type
    if (t == TYPE_U64 || t == TYPE_S64) return 8;
    if (t == TYPE_U32 || t == TYPE_S32) return 4;
    if (t == TYPE_U16 || t == TYPE_S16) return 2;
    if (t == TYPE_U8 || t == TYPE_S8) return 1;
    return 0;
}
```

### 4. Inferring Type from Function Calls

```c
Type* infer_type_from_call(ASTNode* node)
{
    ASTNode* callee = node->as.call.callee;
    
    if (callee->type != AST_IDENTIFIER) {
        report_error(node->location, "Invalid function call");
        return TYPE_ERROR;
    }
    
    char* func_name = callee->as.ident.name;
    Symbol* func_sym = symbol_table_lookup(func_name);
    
    if (func_sym == NULL) {
        report_error(node->location, 
            "Undefined function '%s'", func_name);
        return TYPE_ERROR;
    }
    
    if (func_sym->kind != SYMBOL_FUNCTION) {
        report_error(node->location, 
            "'%s' is not a function", func_name);
        return TYPE_ERROR;
    }
    
    // The function's return type is the type of the call expression
    return func_sym->type->return_type;
}
```

### 5. Inferring Type from Array Index

```c
Type* infer_type_from_index(ASTNode* node)
{
    ASTNode* base = node->as.idx.base;
    ASTNode* index = node->as.idx.index;
    
    // Get type of base
    Type* base_type = infer_type_from_node(base);
    
    if (base_type == TYPE_ERROR) {
        return TYPE_ERROR;
    }
    
    // Base must be array type
    if (base_type->kind != TYPE_ARRAY) {
        report_error(node->location,
            "Cannot index non-array type");
        return TYPE_ERROR;
    }
    
    // Index must be integer
    Type* index_type = infer_type_from_node(index);
    if (!is_integer_type(index_type)) {
        report_error(node->location,
            "Array index must be integer type");
        return TYPE_ERROR;
    }
    
    // Result is the element type of the array
    return base_type->element_type;
}
```

## Putting It All Together: Variable Declaration

Now you can handle both literals and expressions:

```c
case AST_VAR_DECL:
case AST_CONST_DECL:
{
    Token* declared_type = node->as.declaration.data_type;
    ASTNode* identifier = node->as.declaration.ident;
    ASTNode* value = node->as.declaration.value;
    
    Type* final_type = NULL;
    
    // === CASE 1: Explicit type provided ===
    if (declared_type != NULL) {
        final_type = resolve_type_from_token(declared_type);
        
        if (final_type == NULL) {
            report_error(node->location,
                "Unknown type '%s'", declared_type->lexeme);
            return;
        }
        
        // If initializer present, check compatibility
        if (value != NULL) {
            Type* value_type = infer_type_from_node(value);
            
            if (!can_assign(final_type, value_type)) {
                report_error(value->location,
                    "Cannot initialize variable of type '%s' with value of type '%s'",
                    type_to_string(final_type),
                    type_to_string(value_type));
                return;
            }
        }
    }
    // === CASE 2: No explicit type - must infer ===
    else {
        if (value == NULL) {
            report_error(node->location,
                "Variable '%s' must have either a type annotation or initializer",
                identifier->as.ident.name);
            return;
        }
        
        // Infer type from initializer
        final_type = infer_type_from_node(value);
        
        if (final_type == TYPE_ERROR) {
            // Error already reported in recursive call
            return;
        }
    }
    
    // === SEMANTIC CHECK: Constants must be initialized ===
    if (node->type == AST_CONST_DECL && value == NULL) {
        report_error(node->location,
            "Constant '%s' must be initialized",
            identifier->as.ident.name);
        return;
    }
    
    // === Add to symbol table ===
    bool is_const = (node->type == AST_CONST_DECL);
    char* var_name = identifier->as.ident.name;
    
    Symbol* existing = symbol_table_lookup_current_scope(var_name);
    if (existing != NULL) {
        report_error(node->location,
            "Variable '%s' already declared in this scope",
            var_name);
        return;
    }
    
    symbol_table_insert(var_name, final_type, is_const, SYMBOL_VARIABLE);
    
    break;
}
```

## Key Takeaways

1. **Type inference is recursive** - you traverse the expression tree to determine types
2. **Literals have intrinsic types** - determined by their syntax/format
3. **Expressions have computed types** - based on operand types and operators
4. **Variables/identifiers have declared types** - looked up in symbol table
5. **Type checking and inference happen together** - as you infer types, you also validate they're used correctly

The trick is building that `infer_type_from_node()` function that handles all expression forms in your AST!