
#include <stdio.h>
#include <assert.h>

#include "lexer.h"
#include "parser.h"

typedef struct {
  const char* code;
  Token* tokens;
  int tokenCount;
  int nextToken;
} ParseInfo;

#define ATOM(SYNNODE) ((SYNNODE)->type != SYN_LIST)
#define HEAD(SYNNODE) ((SYNNODE)->content.list.head)
#define TAIL(SYNNODE) ((SYNNODE)->content.list.tail)

SyntaxNode* NewList(SyntaxNode* head, SyntaxNode* tail) {
  SyntaxNode* newNode = (SyntaxNode*)Alloc(sizeof(SyntaxNode));
  newNode->type = SYN_LIST;
  HEAD(newNode) = head;
  TAIL(newNode) = tail;
  return newNode;
}

SyntaxNode* NewAtom(SyntaxNodeType type, const char* text, int length) {
  assert(type != SYN_LIST);
  SyntaxNode* newNode = (SyntaxNode*)Alloc(sizeof(SyntaxNode));
  newNode->type = type;
  newNode->content.atom.text = text;
  newNode->content.atom.length = length;
  return newNode;
}

SyntaxNode* ParseAtom(ParseInfo* parseInfo, Token* token) {
  SyntaxNodeType type;
  switch (token->type) {
    case TOK_IDENTIFIER:
    case TOK_KEYWORD_FUN:
    case TOK_KEYWORD_LET:
    case TOK_KEYWORD_NIL:
    case TOK_KEYWORD_OR:
      type = SYN_SYMBOL;
      break;
    case TOK_NUMBER:
      type = SYN_NUMBER;
      break;
    case TOK_STRING:
      type = SYN_STRING;
      break;
    default:
      fprintf(stderr, "Unexpected token type.\n");
      exit(1);
  }
  return NewAtom(type, parseInfo->code + token->offset, token->length);
}

SyntaxNode* ParseList(ParseInfo* parseInfo) {
  SyntaxNode* listHead = 0;
  SyntaxNode* listTail = 0;
  for (;;) {
    Token* nextToken = &parseInfo->tokens[parseInfo->nextToken];
    parseInfo->nextToken++;
    if (nextToken->type == TOK_EOF || nextToken->type == TOK_RPAREN) {
      break;
    }
    SyntaxNode* newNode;
    if (nextToken->type == TOK_LPAREN) {
      newNode = ParseList(parseInfo);
      if (parseInfo->nextToken == parseInfo->tokenCount) {
        fprintf(stderr, "Premature end of file.\n");
        exit(1);
      }
    } else {
      newNode = ParseAtom(parseInfo, nextToken);
    }
    SyntaxNode* newListPair = NewList(newNode, 0);
    if (!listHead) {
      listHead = newListPair;
      listTail = newListPair;
    } else {
      TAIL(listTail) = newListPair;
      listTail = TAIL(listTail);
    }
  }
  return listHead;
}

SyntaxNode* Parse(const char* code, Token* tokens, int tokenCount) {
  ParseInfo parseInfo;
  parseInfo.code = code;
  parseInfo.tokens = tokens;
  parseInfo.tokenCount = tokenCount;
  parseInfo.nextToken = 0;
  SyntaxNode* program = ParseList(&parseInfo);
  if (parseInfo.nextToken < parseInfo.tokenCount) {
    fprintf(stderr, "Unmatched right parenthesis.\n");
    exit(1);
  }
  return program;
}

void PrintAtomText(SyntaxNode* atom) {
  fwrite(atom->content.atom.text, 1, atom->content.atom.length, stdout);
}

void PrintAtom(SyntaxNode* atom) {
  assert(ATOM(atom));
  switch (atom->type) {
    case SYN_SYMBOL:
    case SYN_STRING:
    case SYN_NUMBER:
      PrintAtomText(atom);
      break;
    case SYN_LIST:
      ; /* Won't occur, checked by assert. */
  }
}

void PrintList(SyntaxNode* list) {
  SyntaxNode* node = list;
  while (node) {
    if (node != list)
      printf(" ");
    SyntaxNode* head = HEAD(node);
    if (ATOM(head)) {
      PrintAtom(head);
    } else {
      printf("(");
      PrintList(head);
      printf(")");
    }
    node = TAIL(node);
  }
}

void PrintProgram(SyntaxNode* program) {
  PrintList(program);
}

