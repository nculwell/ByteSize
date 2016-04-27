
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage:\n");
    exit(1);
  }
  const char* filename = argv[1];
  const char* code = LoadFile(filename);
  Token* tokens;
  Lex(code, &tokens);
}

