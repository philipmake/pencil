#ifndef UTILS_H_
#define UTILS_H_


void print_indent(int indent);

void print_ast(ASTNode* node, int indent);

void print_statements(ASTNode** stmts, unsigned int count, int indent);

#endif