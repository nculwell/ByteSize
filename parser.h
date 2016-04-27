
typedef struct {
  const char* code;
  Token* tokens;
  int tokenCount;
  int nextToken;
} ParseInfo;

struct SyntaxNode;

typedef struct {
  int type;
  const char* text;
  int length;
} Atom;

typedef struct List {
  struct SyntaxNode* head;
  struct List* tail;
} List;

typedef struct SyntaxNode {
  int isAtom;
  union {
    Atom* atom;
    List* list;
  } node;
} SyntaxNode;

List* Parse(const char* code, Token* tokens, int tokenCount);
void PrintProgram(List* program);

