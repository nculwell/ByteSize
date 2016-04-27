
#include <malloc.h>

enum TokenType {
  TOK_EOF = 0,
  TOK_IDENTIFIER,
  TOK_STRING,
  TOK_NUMBER,
  TOK_LPAREN,
  TOK_RPAREN,
  TOK_ERROR,
  TOK_COUNT // dummy type used to count the number of values
};

typedef struct {
  enum TokenType type;
  int offset;
  int length;
} Token;

const char* LoadFile(const char*);
int Lex(const char*, Token** tokens);


