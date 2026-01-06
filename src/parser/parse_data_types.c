#include "ast.h"
#include "scope.h"
#include "symtab.h"
#include "parser.h"

// implement data structures like structs, enums, variants, and vec! and list
// parser for parameters, enums, structs, variant, vec, List

ASTNode* parse_field(Parser* parser)
{
    ASTNode* ident = parse_primary_expr(parser);

    parser_consume(parser, COLON, "Expected ':' after parameter name.\n");

    Token* type = parser_advance(parser);

    return ast_field(ident, type);
}

ASTNode* parse_struct(Parser* parser) 
{
    parser_advance(parser); // consume struct keyword
    ASTNode* name = NULL;
    ASTNode** fields = NULL;
    size_t fields_count = 0;

    name = parse_primary_expr(parser);

    if (!parser_check(parser, OPEN_CURLY)) return NULL;
    parser_consume(parser, OPEN_CURLY, "Expected '{' after struct name\n");

    if (!parser_check(parser, CLOSE_CURLY)) 
    {
        do {
            ASTNode* field = parse_field(parser);
            printf("filed333\n");
            fields = parser_grow_array(fields, &fields_count, field);
        } while (parser_match(parser, COMMA));
    }
    parser_consume(parser, CLOSE_CURLY, "Expected '}' after fields\n");

    return ast_struct(name, fields, fields_count);
}

ASTNode* parse_enum(Parser* parser)
{
    parser_consume(parser, ENUM, "Expected 'enum' keyword");
    
    Token* name_token = parser_consume(parser, IDENTIFIER, "Expected enum name after 'enum'");
    if (!name_token) return NULL;
    
    ASTNode* enum_name = ast_new_identifier(name_token);
    
    parser_consume(parser, OPEN_CURLY, "Expected '{' after enum name");
    
    ASTNode** enum_values = NULL;
    size_t enum_count = 0;
    
    // Parse enum variants
    while (!parser_check(parser, CLOSE_CURLY) && !parser_is_at_end(parser))
    {
        // Token* variant_token = parser_advance(parser);

        // if (!variant_token)
            // break;
    
        parser_advance(parser);
        ASTNode* variant = parse_primary_expr(parser);
        enum_values = parser_grow_array(enum_values, &enum_count, variant);
        
        // Optional comma
        if (parser_check(parser, COMMA))
        {
            parser_advance(parser);
            // Allow trailing comma
            if (parser_check(parser, CLOSE_CURLY))
                break;
        }
        else if (!parser_check(parser, CLOSE_CURLY))
        {
            parser_error(parser, "Expected ',' or '}' after enum variant\n");
            break;
        }
    }
    
    parser_consume(parser, CLOSE_CURLY, "Expected '}' after enum variants\n");
    
    return ast_enum(enum_name, enum_values, enum_count);
}

ASTNode* parse_union(Parser* parser) 
{
    parser_advance(parser); // consume variant keyword
    ASTNode* name = NULL;
    ASTNode** fields = NULL;
    size_t fields_count = 0;

    if (!parser_check(parser, OPEN_CURLY)) return NULL;
    parser_consume(parser, OPEN_CURLY, "Expected '{' after struct name\n");

    if (!parser_check(parser, CLOSE_CURLY)) 
    {
        do {
            ASTNode* field = parse_field(parser);
            fields = parser_grow_array(fields, &fields_count, field);
        } while (parser_match(parser, COMMA));
    }
    parser_consume(parser, CLOSE_CURLY, "Expected '}' after fields\n");

    Token* struct_name = parser_advance(parser);
    name = ast_new_identifier(struct_name);
    
    return ast_union(name, fields, fields_count);
}

ASTNode* parse_vec(Parser* parser) 
{

}

ASTNode* parse_list(Parser* parser)
{

}
