
#include <stdio.h>
#include <stdlib.h>

const char* LoadFile(const char*);
void Lex(const char*);

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage:\n");
    exit(1);
  }
  const char* filename = argv[1];
  const char* code = LoadFile(filename);
  Lex(code);
}

