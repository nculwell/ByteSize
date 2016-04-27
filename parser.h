
/*
typedef enum {
  SYN_LIST,
  SYN_SYMBOL,
  SYN_STRING,
  SYN_NUMBER
} SyntaxNodeType;

typedef struct SyntaxNode {
  SyntaxNodeType type;
  union {
    struct {
      const char* text;
      int length;
    } atom;
    struct {
      struct SyntaxNode* head;
      struct SyntaxNode* tail;
    } list;
  } content;
} SyntaxNode;
*/

Term* Parse(const char* code, Token* tokens, int tokenCount);
void PrintProgram(Term* program);

