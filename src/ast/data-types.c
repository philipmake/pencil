#include "ast.h"


ASTNode* ast_field(ASTNode* ident, Token* type)
{
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode));
    n->type = AST_FIELD;
    n->as.field.ident = ident;
    n->as.field.type = type;
    n->location = ident->location;
    return n;    
}

ASTNode* ast_struct(ASTNode* name, ASTNode** fields, size_t fields_count)
{
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode));
    n->type = AST_STRUCT;
    n->as.structType.name = name;
    n->as.structType.fields = fields;
    n->as.structType.fields_count = fields_count;
    n->location = name->location;
    return n;
}

ASTNode* ast_union(ASTNode* name, ASTNode** fields, size_t fields_count)
{
    ASTNode* n = (ASTNode*)parser_alloc(sizeof(ASTNode));
    n->type = AST_UNION;
    n->as.structType.name = name;
    n->as.structType.fields = fields;
    n->as.structType.fields_count = fields_count;
    n->location = name->location;
    return n;
}

ASTNode* ast_enum(ASTNode* name, ASTNode** values, size_t count)
{
    ASTNode* n = malloc(sizeof(ASTNode));
    n->type = AST_ENUM;
    n->as.enumType.enum_name = name;
    n->as.enumType.enum_values = values;
    n->as.enumType.enum_count = count;
    return n;
}