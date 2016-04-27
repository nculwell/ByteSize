
enum TokenType {
  TOK_EOF = 0,
  TOK_IDENTIFIER,
  TOK_NUMBER,
  TOK_STRING,
  TOK_LPAREN,
  TOK_RPAREN,
  TOK_ERROR,
  TOK_KEYWORD, // dummy type used to calculate keyword IDs
  TOK_KEYWORD_FUN,
  TOK_KEYWORD_LET,
  TOK_KEYWORD_NIL,
  TOK_KEYWORD_OR,
  TOK_COUNT // dummy type used to count the number of values
};

typedef struct {
  enum TokenType type;
  int offset;
  int length;
} Token;

const char* LoadFile(const char*);
int Lex(const char*, Token** tokens);

