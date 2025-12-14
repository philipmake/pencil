#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "token.h"
#include "parser.h"
#include "ast.h"
#include "utils.h"

char* filename;

int main(int argc, char* argv[]) 
{
    printf("Starting frontend to scanner file.\n");

    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return -1;
    }
    
    filename = argv[1];
    char path[256];
    strcpy(path, "test/");
    strcat(path, filename);

    Lexer* lex = lexer_init(path);
    if (lex == NULL) {
        printf("Error: lexer not initialised properly.\n");
        return -1;
    }
    
    init_global_array();
    
    lexer(lex);

    print_all_tokens_global(); 

    Parser* parser = init_parser(global_array->tokens, global_array->token_count);
    
    ASTNode* root = parse_program(parser);
    if (root != NULL) {
        printf("\nParsing successful\n\n");
        print_ast(root, 0);
    } else {
        printf("Parsing failed: %s\n", parser->error_msg);
    }

    // init semantic analysis

    // code generation - rv64


    return 0;
}

