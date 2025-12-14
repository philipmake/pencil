# Pencil Programming Language Specification

This is the specification for the Pencil programming language. This document is still in active work.

## 1. Introduction

### 1.1 Purpose of this Document

This document serves as the authoritative reference for the Pencil programming language. It is intended for multiple audiences: compiler implementers who need precise technical specifications, developers who want to write Pencil programs, educators teaching programming concepts, and tool developers building IDEs and other development tools. The specification aims to be both technically rigorous and pedagogically accessible.

### 1.2 Goals and Philosophy

Pencil is designed with several core principles in mind. Safety is paramount—the language provides strong typing and clear semantics to help developers avoid common programming errors. Simplicity guides the design, keeping the language approachable while still being powerful enough for real-world applications. Pencil targets systems programming domains where control over resources matters, yet remains versatile enough for general-purpose development. The language emphasizes clarity and maintainability over clever tricks.

### 1.3 Non-Goals

Pencil does not prioritize maximum performance or aggressive optimizations at the cost of simplicity. While the language should compile to efficient code, extreme optimization is not a primary concern. The focus remains on creating readable, maintainable programs that work correctly.

## 2. Lexical Structure

### 2.1 Character Set

Pencil uses the ASCII character set as its foundation. The language recognizes decimal digits from zero through nine, lowercase letters from a through z, and uppercase letters from A through Z. Additionally, Pencil supports a variety of symbols and operators including semicolons, colons, equals signs, arithmetic operators (plus, minus, asterisk, slash, percent), parentheses, braces, brackets, question marks, angle brackets, commas, periods, forward slashes, at signs, hash symbols, dollar signs, carets, ampersands, asterisks, pipes, exclamation points, tildes, double quotes, and single quotes.

### 2.2 Tokens

The Pencil lexer recognizes several categories of tokens. Keywords are reserved words that have special meaning in the language, including `var`, `let`, `struct`, `enum`, `vec`, `variant`, `if`, `else`, `match`, `loop`, `for`, `in`, `import`, `fn`, `main`, and type names like `str`, `char`, `int`, `inth`, `byte`, `short`, `float`, and `double`.

Literals represent constant values directly in source code. Pencil supports decimal integer literals (optionally signed with plus or minus), binary literals prefixed with `0b`, hexadecimal literals prefixed with `0x`, octal literals prefixed with `0o`, floating-point literals with a decimal point, string literals enclosed in double quotes, character literals enclosed in single quotes, and boolean literals `true` and `false`.

Operators perform computations and comparisons. Additive operators are plus and minus, multiplicative operators are asterisk, slash, and percent for modulo. Prefix operators are double-plus and double-minus for increment and decrement, with matching postfix versions. Logical operators include double-ampersand for AND, double-pipe for OR, and exclamation for NOT. Bitwise operators are single ampersand, pipe, caret, tilde, and the bit-shift operators double-angle-brackets left and right. Comparison operators test relationships: double-equals, exclamation-equals, greater-than, less-than, greater-or-equal, and less-or-equal. Assignment operators include simple equals and the compound forms plus-equals, minus-equals, star-equals, slash-equals, and percent-equals. Special operators include the arrow (equals-greater-than) and the optional operator (question mark).

Punctuators provide structure: the dot for member access, ellipsis (three dots) for ranges, underscore for identifiers and wildcards, semicolons to terminate statements, commas to separate items, and various bracket types for grouping and data structures.

Identifiers name things in Pencil programs. An identifier must start with a letter and may continue with any combination of letters, digits, or underscores.

### 2.3 Comments

Pencil supports two comment styles. Single-line comments begin with two forward slashes and continue until the end of the line. Multi-line comments begin with slash-asterisk and end with asterisk-slash, allowing comments to span multiple lines. Comments are ignored by the compiler and exist solely for human readers.

### 2.4 Whitespace

Whitespace characters include the space character, newline (represented as `\n`), carriage return (`\r`), and tab (`\t`). Whitespace separates tokens but is otherwise ignored by the parser, allowing programmers to format code for readability.

## 3. Grammar

### 3.1 Program Structure

A Pencil program consists of a sequence of declarations followed by the end-of-file marker. The program may include import declarations to bring in external code, followed by type definitions like structs, enums, and variants, and then function declarations. One function must be named `main` to serve as the program entry point.

Each declaration can be a variable declaration, constant declaration, array declaration, function declaration, import statement, struct definition, enum definition, or vector declaration. These declarations collectively define the structure and behavior of the program.

### 3.2 Expressions

Expressions in Pencil follow a carefully designed precedence hierarchy. At the top level, an expression can be an assignment. Assignments bind an identifier to a value using one of the assignment operators, and they associate right-to-left.

Below assignments come logical OR operations, which combine expressions using the double-pipe operator and evaluate left-to-right. Logical AND operations (double-ampersand) have higher precedence and also evaluate left-to-right. 

Equality comparisons using double-equals and exclamation-equals come next, followed by relational comparisons using greater-than, less-than, greater-or-equal, and less-or-equal. The range operator (three dots) creates sequences between two values.

Arithmetic operations follow standard mathematical precedence. Additive operations (plus and minus) have lower precedence than multiplicative operations (asterisk, slash, and modulo). 

Unary operations include logical NOT (exclamation), unary plus and minus, and bitwise NOT (tilde). These prefix operators have higher precedence than binary operations.

Postfix operations provide array indexing with square brackets and function calls with parentheses. A function call may include an optional argument list.

Primary expressions are the building blocks: literals (numbers, strings, characters, booleans), identifiers that reference named values, and parenthesized expressions for explicit grouping.

The prefix and postfix increment and decrement operators (double-plus and double-minus) modify their operand's value, returning either the old value (postfix) or new value (prefix).

### 3.3 Statements

Statements are the executable units of Pencil programs. A statement can be a declaration introducing new names, an expression evaluated for its side effects and terminated with a semicolon, a function call, or a control flow construct.

If statements provide conditional execution. They begin with the `if` keyword followed by a boolean expression in parentheses and a block of statements to execute when true. Optional `else if` clauses provide additional conditions to test, and an optional `else` clause executes when all conditions are false.

Loop statements repeat execution. The basic `loop` construct followed by an expression and block creates an iteration structure. The `for` statement provides iteration over ranges or collections using either a range expression or an `in` expression to traverse elements.

Match statements (written with the `case` keyword) provide pattern matching. They evaluate an expression and compare it against multiple patterns, each associated with a block of code. Patterns are separated by commas, and an underscore serves as the wildcard pattern that matches anything, typically placed last as the default case.

Return statements exit from functions, optionally providing a value to return to the caller.

Blocks are sequences of statements enclosed in curly braces, creating a new scope for local variables.

## 4. Types

### 4.1 Primitive Types

Pencil provides several fundamental types. Integer types come in various sizes: `byte` for very small integers, `short` for somewhat larger values, `int` for standard integer arithmetic, `inth` (half-integer-size) for a middle ground, and potentially larger types like `long` and `xlong` for extended ranges. Each can be signed or unsigned.

Floating-point types include `float` for single-precision and `double` for double-precision decimal numbers.

Character types distinguish between single characters (`char`) and strings (`str`). A string type can optionally specify its buffer length using bracket notation like `str[100]` to indicate a string that can hold up to 100 characters.

Boolean types represent truth values using the literals `true` and `false`.

### 4.2 Composite Types

Pencil supports several ways to build complex types from simpler ones. Structs are named collections of fields, each with its own type. A struct declaration gives the struct a name and lists its fields as parameter-style declarations.

Enums define a type that can take on one of several named values, useful for representing a fixed set of alternatives.

Variants (similar to tagged unions) allow a value to be one of several different types, with each alternative labeled by name.

Arrays are fixed-size sequences of elements of the same type. Array declarations specify the element type and either a specific size or a range.

Vectors (declared with `vec!` followed by a type in brackets) provide dynamic collections that can grow and shrink.

### 4.3 Type Conversions

Type conversions in Pencil handle situations where values need to change type. The language distinguishes between implicit conversions that happen automatically when safe (such as widening numeric conversions from smaller to larger types) and explicit conversions that require programmer intent. The specifics of conversion rules ensure type safety while providing necessary flexibility.

## 5. Variables and Scopes

### 5.1 Variable Declarations

Variables in Pencil come in two flavors. Mutable variables are declared with the `var` keyword, followed by an identifier, an optional type annotation introduced with a colon, and an optional initialization expression introduced with an equals sign. The declaration ends with a semicolon.

Constants use the `let` keyword and follow the same syntax pattern. Once initialized, constants cannot be reassigned, though note that for complex types like structs, the internal fields might still be modifiable depending on the type system's rules.

Arrays can be declared as constants with a special syntax that specifies both the element type and the index range or size. An array initializer provides the initial values enclosed in square brackets with comma separation.

### 5.2 Mutability Rules

The distinction between `var` and `let` establishes clear mutability semantics. Variables declared with `var` can be reassigned after initialization, supporting imperative programming styles where values change over time. Variables declared with `let` are bound to their initial value and cannot be reassigned, encouraging functional programming patterns and making code easier to reason about.

The type system may extend mutability control to struct fields and other composite types, though the specifics would need further specification.

### 5.3 Scoping Rules

Pencil uses lexical scoping where each pair of curly braces creates a new scope. Variables declared inside a block are visible only within that block and any nested blocks. When a block ends, its local variables go out of scope and become inaccessible.

Name shadowing allows inner scopes to declare variables with the same name as outer scope variables, temporarily hiding the outer variable. The outer variable remains unchanged and becomes visible again when the inner scope ends.

Function parameters create their own scope for the function body. Top-level declarations are visible throughout the entire file following their declaration.

## 6. Functions

### 6.1 Function Declarations and Definitions

Functions in Pencil are declared using the `fn` keyword, followed by the function name, a parameter list in parentheses, an optional return type introduced by an arrow (`->`), and a function body enclosed in curly braces. 

The function body consists of a sequence of blocks (each being a sequence of statements) and ends with an optional return statement. If no explicit return statement appears, functions implicitly return at the end of their body.

The `main` function serves as the program entry point. It must be present in every executable Pencil program and is called when the program starts.

### 6.2 Parameters

Function parameters are listed in the parentheses following the function name. Each parameter consists of an identifier, a colon, and a type. Multiple parameters are separated by commas. Parameters are immutable within the function body—they cannot be reassigned, though for mutable types the contents might be modifiable.

Functions without parameters have empty parentheses. The parameter syntax mirrors the type annotation syntax used in variable declarations, creating consistency across the language.

### 6.3 Return Semantics

Functions can return values to their callers using the `return` keyword followed by an expression. The expression's type must match the declared return type. Functions can have multiple return statements along different execution paths.

If a function's return type is omitted from the declaration, it implicitly returns no value (similar to void in other languages). Attempting to return a value from such a function or use it in an expression context would be an error.

## 7. Control Flow

### 7.1 Conditionals

The `if` statement provides branching based on boolean conditions. The condition expression must appear in parentheses and is followed by a block of statements to execute when the condition is true. 

Multiple conditions can be tested in sequence using `else if` clauses, each with its own condition and block. Finally, an `else` clause without a condition provides a default block that executes when all previous conditions are false.

The blocks use curly braces even for single statements, maintaining consistency and preventing errors when statements are added later.

### 7.2 Loops

Pencil provides two main loop constructs. The `loop` statement creates a basic loop that combines a condition expression with a block of statements to repeat. The exact semantics of how the expression controls the loop would need further specification—it might be a count, a condition, or something else.

The `for` statement provides iteration, accepting either a range expression (like `0...10`) or an `in` expression to iterate over a collection. The loop variable takes on each value in sequence, and the block executes for each iteration.

### 7.3 Pattern Matching

The match statement (introduced with the `case` keyword) provides powerful pattern-based dispatch. It evaluates a pattern expression, then compares it against a series of cases. Each case consists of an expression representing the pattern to match, a colon, and a block of statements to execute if the match succeeds, followed by a comma.

Multiple cases can be provided, and the underscore serves as a catch-all pattern that matches anything. This wildcard typically appears as the last case to handle all remaining possibilities, ensuring exhaustive matching.

### 7.4 Exceptions / Error Handling

Error handling mechanisms in Pencil are not fully specified in the current grammar. The language might adopt explicit return-value-based error handling, exception mechanisms, or result types that combine success and error cases. This section awaits further design decisions.

## 8. Memory Management

Memory management strategies for Pencil remain to be specified. The language might use automatic garbage collection, manual memory management with explicit allocation and deallocation, reference counting, or ownership-based systems like Rust's borrow checker. The choice will significantly impact how developers write Pencil programs and will need careful consideration based on the language's systems programming goals.

## 9. Standard Library

The Pencil standard library is not yet defined. A systems programming language typically provides facilities for input/output operations, string manipulation, basic data structures like vectors and hash maps, mathematical functions, and system interaction. As Pencil develops, this section will document the available library functions and types.

## 10. Tests

Testing methodology and any built-in testing facilities are not yet specified. The language might provide special syntax for writing unit tests, a testing framework in the standard library, or rely on external testing tools. Best practices for testing Pencil programs will be documented here as they emerge.

---

*This specification is a living document and will evolve as the Pencil language develops. Future revisions will expand on areas currently marked as incomplete and refine the semantics of existing features.*