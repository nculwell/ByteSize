
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

SyntaxNode* Parse(const char* code, Token* tokens, int tokenCount);
void PrintProgram(SyntaxNode* program);

enum DataType {
  TYPE_LIST,
  TYPE_PRIMITIVE,
  TYPE_SYMBOL,
  TYPE_STRING,
  TYPE_NUMBER
};

