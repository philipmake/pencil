Excellent! Since you already have symbol tables and scoping, let me give you a more comprehensive semantic analysis framework.

## Comprehensive Semantic Rules

### **Core Semantic Checks by Category**

#### **1. Declaration Analysis**

**Variables & Constants:**
- Type compatibility between declared type and initializer
- Constants must be initialized at declaration
- Variables cannot be declared with `void` type
- Array dimensions must be compile-time constants
- Duplicate parameter names in function declarations
- Shadowing warnings (optional but helpful)

**Functions:**
- No duplicate function names in same scope (no overloading in your language)
- Parameter types must be valid
- Return type must match all return statements
- Functions without return type (void) cannot return values
- Non-void functions must have at least one return on all paths
- `main` function must exist and have correct signature

**Structs/Enums/Unions:**
- No duplicate field names within same struct/union
- No duplicate enum variant names
- Recursive struct definitions must use pointers (if you support them)
- Field types must be fully defined (no forward references without pointers)

#### **2. Expression Analysis**

**Type Checking:**
- Binary operations require compatible types:
  - Arithmetic (`+`, `-`, `*`, `/`, `%`): numeric types only
  - Modulo (`%`): integer types only
  - Bitwise (`&`, `|`, `^`, `<<`, `>>`): integer types only
  - Logical (`&&`, `||`): boolean operands
  - Comparison (`==`, `!=`): same types
  - Relational (`<`, `>`, `<=`, `>=`): numeric types
- Unary operations:
  - `!`: boolean operand
  - `~`: integer operand
  - `+`, `-`: numeric operand
- Implicit type conversions (if you allow them):
  - Widening conversions (u8 → u16 → u32 → u64)
  - Signed/unsigned compatibility rules

**Lvalue Analysis:**
- Assignment target must be an lvalue (variable, array element, struct field)
- Cannot assign to constants
- Cannot assign to function names or enum variants

**Function Calls:**
- Callee must be a function
- Argument count must match parameter count
- Argument types must match parameter types (exactly or via conversion)
- Cannot call before declaration (or require forward declarations)

**Array/Index Operations:**
- Base must be array type
- Index must be integer type
- Index must be within bounds (if statically determinable)

**Range Expressions:**
- Start and end must be integer types
- Step (if present) must be integer and non-zero
- Start must be less than end (for positive step)

#### **3. Statement Analysis**

**Control Flow:**
- `break`/`continue` only valid inside loops
- Return statement type must match function return type
- All code paths in non-void functions must return
- Unreachable code detection (statements after unconditional return)

**Match Statements:**
- Pattern must be matchable type (integer, enum, char)
- All match arms must produce same type (if used as expression)
- Exhaustiveness checking for enums
- No duplicate case patterns
- Default case (`_`) should be last

**If Statements:**
- Condition must be boolean type
- If used as expression, both branches must return same type

**Loop Statements:**
- Loop condition (if present) must be boolean
- Loop range expressions must be valid

#### **4. Advanced Checks**

**Definite Assignment:**
- Variables must be initialized before use
- Track which variables are definitely assigned on each code path
- Warn about potentially uninitialized variables

**Const Correctness:**
- Cannot modify const variables after initialization
- Cannot pass const variables to functions expecting mutable references

**Type Width Checks:**
- Integer literal fits in declared type
- No loss of precision in implicit conversions
- Overflow detection for compile-time constants

## Robust Example: Function Declaration Analysis

```c
case AST_FN_DECL:
{
    ASTNode* fn_name = node->as.func.ident;
    Token* return_type_tok = node->as.func.return_type;
    ASTNode** params = node->as.func.params;
    size_t param_count = node->as.func.params_count;
    ASTNode* body = node->as.func.block;
    
    // === SEMANTIC CHECK 1: Function name collision ===
    Symbol* existing = symbol_table_lookup_current_scope(fn_name->as.ident.name);
    if (existing != NULL) {
        report_error(node->location, 
            "Function '%s' is already declared in this scope", 
            fn_name->as.ident.name);
        return;
    }
    
    // === SEMANTIC CHECK 2: Validate return type ===
    Type* return_type = NULL;
    if (return_type_tok != NULL) {
        return_type = resolve_type_from_token(return_type_tok);
        if (return_type == NULL) {
            report_error(node->location, 
                "Unknown return type '%s'", 
                return_type_tok->lexeme);
            return;
        }
    } else {
        return_type = TYPE_VOID; // Default to void if not specified
    }
    
    // === SEMANTIC CHECK 3: Enter function scope ===
    symbol_table_enter_scope(SCOPE_FUNCTION);
    
    // === SEMANTIC CHECK 4: Validate and register parameters ===
    Type** param_types = malloc(sizeof(Type*) * param_count);
    for (size_t i = 0; i < param_count; i++) {
        ASTNode* param = params[i];
        if (param->type != AST_PARAM) {
            report_error(param->location, "Expected parameter node");
            continue;
        }
        
        char* param_name = param->as.param.ident->as.ident.name;
        Token* param_type_tok = param->as.param.type;
        
        // Check for duplicate parameter names
        Symbol* dup = symbol_table_lookup_current_scope(param_name);
        if (dup != NULL) {
            report_error(param->location, 
                "Duplicate parameter name '%s'", 
                param_name);
            continue;
        }
        
        // Resolve parameter type
        Type* param_type = resolve_type_from_token(param_type_tok);
        if (param_type == NULL) {
            report_error(param->location, 
                "Unknown parameter type '%s'", 
                param_type_tok->lexeme);
            param_type = TYPE_ERROR; // Use error type to continue analysis
        }
        
        // Check for void parameter (invalid)
        if (param_type == TYPE_VOID) {
            report_error(param->location, 
                "Parameter '%s' cannot have void type", 
                param_name);
        }
        
        param_types[i] = param_type;
        
        // Add parameter to symbol table
        symbol_table_insert(param_name, param_type, false, SYMBOL_VARIABLE);
    }
    
    // === SEMANTIC CHECK 5: Create function type and register ===
    // Exit scope temporarily to add function to parent scope
    symbol_table_exit_scope();
    
    Type* function_type = create_function_type(return_type, param_types, param_count);
    symbol_table_insert(fn_name->as.ident.name, function_type, false, SYMBOL_FUNCTION);
    
    // Re-enter function scope for body analysis
    symbol_table_enter_scope(SCOPE_FUNCTION);
    // Re-add parameters
    for (size_t i = 0; i < param_count; i++) {
        ASTNode* param = params[i];
        char* param_name = param->as.param.ident->as.ident.name;
        symbol_table_insert(param_name, param_types[i], false, SYMBOL_VARIABLE);
    }
    
    // === SEMANTIC CHECK 6: Analyze function body ===
    // Set current function context for return statement checking
    set_current_function_return_type(return_type);
    
    bool has_return = false;
    if (body != NULL && body->type == AST_BLOCK) {
        for (size_t i = 0; i < body->as.block.count; i++) {
            ASTNode* stmt = body->as.block.statements[i];
            analyse_node(stmt);
            
            // Check if we've seen a return statement
            if (stmt->type == AST_RETURN) {
                has_return = true;
                
                // Check for unreachable code
                if (i < body->as.block.count - 1) {
                    report_warning(body->as.block.statements[i + 1]->location,
                        "Unreachable code after return statement");
                }
            }
        }
    }
    
    // === SEMANTIC CHECK 7: Verify return requirement ===
    if (return_type != TYPE_VOID && !has_return) {
        report_error(node->location,
            "Function '%s' with non-void return type must return a value on all paths",
            fn_name->as.ident.name);
    }
    
    // If return type is void, check that no return statements have values
    // This would be checked in AST_RETURN case
    
    clear_current_function_return_type();
    symbol_table_exit_scope();
    
    free(param_types);
    break;
}
```

## Example: Binary Expression Type Checking

```c
case AST_BINARY:
{
    ASTNode* left = node->as.binary.left;
    ASTNode* right = node->as.binary.right;
    Token* op = node->as.binary.op;
    
    // Recursively analyze operands and get their types
    Type* left_type = analyse_expression(left);
    Type* right_type = analyse_expression(right);
    
    if (left_type == TYPE_ERROR || right_type == TYPE_ERROR) {
        // Propagate error type
        return TYPE_ERROR;
    }
    
    // === SEMANTIC CHECK: Operator-specific type rules ===
    switch (op->type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_STAR:
        case TOKEN_SLASH:
        {
            // Arithmetic: both operands must be numeric
            if (!is_numeric_type(left_type)) {
                report_error(left->location,
                    "Left operand of '%s' must be numeric type, got '%s'",
                    op->lexeme, type_to_string(left_type));
                return TYPE_ERROR;
            }
            if (!is_numeric_type(right_type)) {
                report_error(right->location,
                    "Right operand of '%s' must be numeric type, got '%s'",
                    op->lexeme, type_to_string(right_type));
                return TYPE_ERROR;
            }
            
            // Result type is the "wider" of the two types
            Type* result_type = arithmetic_conversion(left_type, right_type);
            
            // Store resolved type on node for code generation phase
            node->resolved_type = result_type;
            return result_type;
        }
        
        case TOKEN_PERCENT: // Modulo
        {
            // Both must be integer types
            if (!is_integer_type(left_type) || !is_integer_type(right_type)) {
                report_error(node->location,
                    "Modulo operator requires integer operands");
                return TYPE_ERROR;
            }
            
            Type* result_type = arithmetic_conversion(left_type, right_type);
            node->resolved_type = result_type;
            return result_type;
        }
        
        case TOKEN_AMPERSAND:
        case TOKEN_PIPE:
        case TOKEN_CARET:
        case TOKEN_LSHIFT:
        case TOKEN_RSHIFT:
        {
            // Bitwise: both must be integer types
            if (!is_integer_type(left_type) || !is_integer_type(right_type)) {
                report_error(node->location,
                    "Bitwise operator '%s' requires integer operands",
                    op->lexeme);
                return TYPE_ERROR;
            }
            
            Type* result_type = arithmetic_conversion(left_type, right_type);
            node->resolved_type = result_type;
            return result_type;
        }
        
        case TOKEN_EQUAL_EQUAL:
        case TOKEN_BANG_EQUAL:
        {
            // Equality: types must be compatible
            if (!types_are_compatible(left_type, right_type)) {
                report_error(node->location,
                    "Cannot compare incompatible types '%s' and '%s'",
                    type_to_string(left_type), type_to_string(right_type));
                return TYPE_ERROR;
            }
            
            node->resolved_type = TYPE_BOOL;
            return TYPE_BOOL;
        }
        
        case TOKEN_LESS:
        case TOKEN_GREATER:
        case TOKEN_LESS_EQUAL:
        case TOKEN_GREATER_EQUAL:
        {
            // Relational: both must be numeric
            if (!is_numeric_type(left_type) || !is_numeric_type(right_type)) {
                report_error(node->location,
                    "Relational operator '%s' requires numeric operands",
                    op->lexeme);
                return TYPE_ERROR;
            }
            
            node->resolved_type = TYPE_BOOL;
            return TYPE_BOOL;
        }
        
        case TOKEN_AND_AND:
        case TOKEN_OR_OR:
        {
            // Logical: both must be boolean
            if (left_type != TYPE_BOOL) {
                report_error(left->location,
                    "Left operand of logical operator must be boolean");
                return TYPE_ERROR;
            }
            if (right_type != TYPE_BOOL) {
                report_error(right->location,
                    "Right operand of logical operator must be boolean");
                return TYPE_ERROR;
            }
            
            node->resolved_type = TYPE_BOOL;
            return TYPE_BOOL;
        }
        
        default:
            report_error(node->location, "Unknown binary operator");
            return TYPE_ERROR;
    }
}
```

## Example: Assignment Lvalue Checking

```c
case AST_ASSIGN:
{
    Token* name_tok = node->as.assign.name;
    Token* op = node->as.assign.op;
    ASTNode* value = node->as.assign.value;
    
    // === SEMANTIC CHECK 1: Variable must exist ===
    Symbol* sym = symbol_table_lookup(name_tok->lexeme);
    if (sym == NULL) {
        report_error(node->location,
            "Undefined variable '%s'", name_tok->lexeme);
        return TYPE_ERROR;
    }
    
    // === SEMANTIC CHECK 2: Cannot assign to constants ===
    if (sym->is_const) {
        report_error(node->location,
            "Cannot assign to constant '%s'", name_tok->lexeme);
        return TYPE_ERROR;
    }
    
    // === SEMANTIC CHECK 3: Cannot assign to functions ===
    if (sym->kind == SYMBOL_FUNCTION) {
        report_error(node->location,
            "Cannot assign to function '%s'", name_tok->lexeme);
        return TYPE_ERROR;
    }
    
    // === SEMANTIC CHECK 4: Type compatibility ===
    Type* value_type = analyse_expression(value);
    
    if (value_type == TYPE_ERROR) {
        return TYPE_ERROR;
    }
    
    // For compound assignments (+=, -=, etc.), verify operation is valid
    if (op->type != TOKEN_EQUAL) {
        // Extract the underlying operation
        TokenType underlying_op = compound_to_binary_op(op->type);
        
        // Check if operation is valid for these types
        if (!is_valid_binary_operation(underlying_op, sym->type, value_type)) {
            report_error(node->location,
                "Invalid compound assignment: cannot apply '%s' to types '%s' and '%s'",
                op->lexeme, type_to_string(sym->type), type_to_string(value_type));
            return TYPE_ERROR;
        }
    }
    
    // Check type compatibility
    if (!can_assign(sym->type, value_type)) {
        report_error(node->location,
            "Type mismatch: cannot assign '%s' to variable of type '%s'",
            type_to_string(value_type), type_to_string(sym->type));
        return TYPE_ERROR;
    }
    
    // Assignment produces the type of the variable
    node->resolved_type = sym->type;
    return sym->type;
}
```

## Additional Semantic Rules to Consider

**Constant Expression Evaluation:**
- Array sizes must be constant expressions
- Enum values should be constant integers
- Case labels in match statements must be constants

**Definite Assignment Example:**
```c
// Track which variables are initialized
typedef struct {
    char* var_name;
    bool is_initialized;
} InitStatus;

// Before using a variable in an expression:
if (!is_definitely_initialized(var_name)) {
    report_error(location, 
        "Variable '%s' may not have been initialized", var_name);
}
```

**Control Flow Return Path Checking:**
```c
// Returns true if all paths return a value
bool check_all_paths_return(ASTNode* block) {
    // Implement flow analysis:
    // - Sequential statements: check if any returns
    // - If statements: both branches must return
    // - Match statements: all cases must return
    // - Loops: cannot guarantee return (might not execute)
}
```

This should give you a solid foundation for robust semantic analysis! The key is being thorough and catching errors early with good error messages.