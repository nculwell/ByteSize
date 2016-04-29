
#include <stdio.h>
#include <stdlib.h>

#include "datatype.h"
#include "lexer.h"
#include "parser.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage:\n");
    exit(1);
  }
  printf("sizeof(Term) = %d\n", sizeof(Term));
  printf("sizeof(Env) = %d\n", sizeof(Env));
  const char* filename = argv[1];
  const char* code = LoadFile(filename);
  Token* tokens;
  int tokenCount = Lex(code, &tokens);
  Term* program = Parse(code, tokens, tokenCount);
  PrintProgram(program);
  Interpret(program);
}

