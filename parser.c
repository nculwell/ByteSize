
#include <stdio.h>
#include <assert.h>

#include "lexer.h"

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

List* NewList(SyntaxNode* head, List* tail) {
  List* newList = (List*)Alloc(sizeof(List));
  newList->head = head;
  newList->tail = tail;
  return newList;
}

SyntaxNode* NewSyntaxNode(Atom* atom, List* list) {
  assert(!atom || !list);
  assert(atom || list);
  SyntaxNode* newNode = (SyntaxNode*)Alloc(sizeof(SyntaxNode));
  newNode->isAtom = !list;
  if (!list)
    newNode->node.atom = atom;
  else
    newNode->node.list = list;
  return newNode;
}

Atom* ParseAtom(ParseInfo* parseInfo, Token* token) {
  Atom* atom = (Atom*)Alloc(sizeof(Atom));
  atom->type = token->type;
  atom->text = parseInfo->code + token->offset;
  atom->length = token->length;
  return atom;
}

List* ParseList(ParseInfo* parseInfo) {
  List* listHead = 0;
  List* listTail = 0;
  for (;;) {
    Token* nextToken = &parseInfo->tokens[parseInfo->nextToken];
    parseInfo->nextToken++;
    if (nextToken->type == TOK_EOF || nextToken->type == TOK_RPAREN) {
      break;
    }
    SyntaxNode* newNode;
    if (nextToken->type == TOK_LPAREN) {
      newNode = NewSyntaxNode(0, ParseList(parseInfo));
      if (parseInfo->nextToken == parseInfo->tokenCount) {
        fprintf(stderr, "Premature end of file.\n");
        exit(1);
      }
    } else {
      newNode = NewSyntaxNode(ParseAtom(parseInfo, nextToken), 0);
    }
    List* newListPair = NewList(newNode, 0);
    if (!listHead) {
      listHead = newListPair;
      listTail = newListPair;
    } else {
      listTail->tail = newListPair;
      listTail = listTail->tail;
    }
  }
  return listHead;
}

List* Parse(const char* code, Token* tokens, int tokenCount) {
  ParseInfo parseInfo;
  parseInfo.code = code;
  parseInfo.tokens = tokens;
  parseInfo.tokenCount = tokenCount;
  parseInfo.nextToken = 0;
  List* program = ParseList(&parseInfo);
  if (parseInfo.nextToken < parseInfo.tokenCount) {
    fprintf(stderr, "Unmatched right parenthesis.\n");
    exit(1);
  }
  return program;
}

void PrintAtomText(Atom* atom) {
  fwrite(atom->text, 1, atom->length, stdout);
}

void PrintAtom(Atom* atom) {
  switch (atom->type) {
    case TOK_IDENTIFIER:
    case TOK_NUMBER:
    case TOK_STRING:
    case TOK_KEYWORD_FUN:
    case TOK_KEYWORD_LET:
    case TOK_KEYWORD_NIL:
    case TOK_KEYWORD_OR:
      PrintAtomText(atom);
      break;
    case TOK_EOF:
    case TOK_ERROR:
    case TOK_KEYWORD:
    case TOK_COUNT:
      fprintf(stderr, "Unexpected token type.\n");
      exit(1);
      break;
  }
}

void PrintList(List* list) {
  List* node = list;
  while (node) {
    if (node != list)
      printf(" ");
    if (node->head->isAtom) {
      PrintAtom(node->head->node.atom);
    } else {
      printf("(");
      PrintList(node->head->node.list);
      printf(")");
    }
    node = node->tail;
  }
}

void PrintProgram(List* program) {
  PrintList(program);
}

