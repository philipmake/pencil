#ifndef ANALYSIS_H_
#define ANALYSIS_H_

#include "ast.h"

void start_analysis(ASTNode* prog);

// type checking
void check_types(ASTNode* node);
void type_errors(char* type_error_list);

// array checking
void array_analyse(ASTNode* array);

// function analysis
void function_analyse(ASTNode* function);

// conditionals
void if_analyse(ASTNode* if_condition);
void match_analyse(ASTNode* matchstmt);

// iteration
void loop_analyse(ASTNode* loops);

// expression
void expr_analyse(ASTNode* expr);
void decl_analyse(ASTNode* decl);


void analyse_node(ASTNode* node);

#endif
