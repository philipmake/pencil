## Operators
<add_op>          = "+" | "-"
<mult_op>         = "*" | "/" | "%"

<prefix_op>       = "++" | "--"
<postfix_op>      = "++" | "--"

<logical_op>      = "&&" | "||" | "!"
<bitwise_op>      = "&" | "|" | "^" | "~" | ">>" | "<<"
<comparison_op>   = "==" | "!=" | ">" | "<" | ">=" | "<=" 
<assign_op>       = "=" | "+=" | "-=" | "*=" | "/=" | "%=" | "/="
<arrow_op>        = "=>"
<optional_op>     = "?"

<dot>             = "."
<ellipsis>        = "..."
<underscore>      = "_"
<esc>             = "\n" | "\t" | "\r"

## Characters
<digit>           = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
<lower_case>      = "a" .. "z"
<upper_case>      = "A" .. "Z"
<letter>          = <lower_case> | <upper_case>

## Literals
<decimal_literal> = ["+" | "-"] <digit>+
<binary_literal>  = "0b" ("0" | "1")+
<hex_literal>     = "0x" ("0".."9" | "A".."F")+
<octal_literal>   = "0o" ("0".."7")+
<float_literal>   = <digit>* "." <digit>+

<string_literal>  = "\"" (<letter> | <digit> | " " | <esc>)* "\""
<char_literal>    = "'" <letter> "'"
<bool_literal>    = "true" | "false"
<literal>         = <decimal_literal> | <float_literal> | <binary_literal> |
                   <hex_literal> | <octal_literal> | <string_literal> |
                   <char_literal> | <bool_literal>

## Whitespace and Comments
<whitespace>      = " " | <esc>
<line_comment>    = "//" (any character)* "\n"
<block_comment>   = "/*" (any character)* "*/"

## Identifier 
<ident> = <letter> (<letter> | <digit> | "_")*

## Expressions
<expr>          -> <assign>
<assign>        -> <ident> <assign_op> <assign> | <logical_or>
<logical_or>    -> <logical_and> ("||" <logical_and>)*
<logical_and>   -> <equality> ("&&" <equality>)*
<equality>      -> <comparison> (("==" | "!=") <comparison>)*
<comparison>    -> <range> ((">" | ">=" | "<" | "<=") <range>)*
<range>         -> <term> ("..." <term>)*
<term>          -> <factor> (("+" | "-") <factor>)*
<factor>        -> <unary> (("*" | "/" | "%") <unary>)*
<unary>         -> ("!" | "+" | "-" | "~") <unary> | <postfix>
<postfix>       -> <primary> ("[" <expr> "]" | "(" <args?> ")")*
<primary>       -> <literal> | <ident> | "(" <expr> ")"

<postfix>       -> <expr> <postfix_op> 
<prefix>        -> <prefix_op> <expr>

## Operator Precedence (High → Low)
1. Parentheses: `()`
2. Unary operators: `++ -- ! ~`
3. Multiplicative: `* / %`
4. Additive: `+ -`
5. Comparison: `== != > < >= <=`
6. Logical: `&& ||`
7. Assignment: `= += -= *= /= %=`

## Keywords
var, let,
struct, enum, vec, union,
if, else, match, 
loop,
import
u8, u16, u32, u64,
s8, s16, s32, s64,
f32, f64,
fn, main, return
break, continue

## Reserved words
String, Int, Float, Float64, Int64,
Char, Bool, type, use, defer


-- string types ::= "str" "[" <int> "]";  This tells the length of the string buffer;
-- type definition ::= "type" types? or data structure definition like enum, struct, variant, vec!, list

<declaration> ::= 
      <var_declaration>
    | <const_declaration>
    | <array_declaration>
    | <function_declaration>
    | <import_declaration>
    | <struct_declaration>
    | <union_declaration>
    | <enum_declaration>
    | <vector_declaration>

<var_declaration>      ::= "var" <ident> [ ":" <type> ] [ "=" <expr> ] 
<const_declaration>    ::= "let" <ident> [ ":" <type> ] [ "=" <expr> ] 
<array_declaration>    ::= "let" | "var" <ident> "[" <type> ":" <range> "]" [ "=" <array_initializer> ]
<import_declaration>   ::= "import" <string_literal> 
<array_initializer>    ::= "[" <literal> ("," <literal>)* "]"

<param> ::= [<ident>":"<type>]*

<function_declaration> ::= "fn" <ident> 
                            "(" [ <param> ("," <param>)* ] ")" 
                            [ "->" <return_type> ] 
                            "{" 
                              <stmt>* 
                              <return_stmt>? 
                            "}"

<field> ::= [<ident>":"<type>]*

<struct_declaration> ::=  "struct" <ident> "{" 
                            <field>* 
                          "}"

<union_declaration> ::= "union" "{" 
                          <field>* 
                        "}" <ident>

<variant> ::= <ident>

<enum_list> ::= <variant> ("," <variant>)*

<enum_declaration>  ::= "enum" <ident> "{" <enum_list> "}"

<body>        ::= <statement>* EOF

<statement>   ::= 
      <declaration>
    | <expr>
    | <func_call> 
    | <if_statement>
    | <loop_statement>
    | <match_statement>
    | <return_statement>
    | <impl>

<block>       ::= "{" <statements>* "}"

<return_stmt>         ::= "return" [ <expr> ]

<if_statement>        ::= "if" "(" <expr> ")" <block>
                          [ <else_if_statement>* ]
                          [ <else_statement> ]

<else_if_statement>   ::= "else if" "(" <expr> ")" <block>

<else_statement>      ::= "else" <block>

<loop_expr>           ::= <ident> ":" <expr> | <expr>

<loop_statement>      ::= "loop" [ <loop_expr> ] <block>

<match_statement>     ::= "match" <pattern> "{"
                              <expr> ":"  <block> | <stmt> ","
                              [<expr> ":" <block> | <stmt> ","]*
                              "_" ":" <blcok> | <stmt>
                          "}"

<args>        ::= <literal> [ ("," <literal>)* ]
<func_call>   ::= <ident> "(" <args> ")"


- for later
## Gnerics

## Implement block
<impl> ::= "impl" <ident> <block>

## Methods on objects
<method_call> ::= <ident> "." <func_call> [ ("." <func_call>)* ]
