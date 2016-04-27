
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "datatype.h"
#include "lexer.h"
#include "parser.h"

typedef struct {
  const char* code;
  Token* tokens;
  int tokenCount;
  int nextToken;
} ParseInfo;

Term* NewList(Term* head, Term* tail) {
  Term* newNode = (Term*)Alloc(sizeof(Term));
  newNode->type = T_LIST;
  HEAD(newNode) = head;
  TAIL(newNode) = tail;
  return newNode;
}

Term* NewAtom(DataType type) {
  assert(TYPE_IS_ATOM(type));
  Term* newAtom = (Term*)Alloc(sizeof(Term));
  newAtom->type = type;
  return newAtom;
}

// TODO: Handle more than just integers.
int ParseNumber(const char* text, int length) {
  char *endptr;
  errno = 0;
  long n = strtol(text, &endptr, 10);
  // Since the string isn't null-terminated,
  // instead of checking that *endptr=='\0' we
  // check if it was advanced the right number
  // of characters.
  if (errno != 0 || endptr - text < length) {
    fprintf(stderr, "Failed to parse number: ");
    fwrite(text, 1, length, stderr);
    fprintf(stderr, "\n");
    if (errno != 0)
      fprintf(stderr, "errno\n");
    else
      fprintf(stderr, "endptr: %d\n", *endptr);
    exit(1);
  }
  if (n < (long)INT_MIN || n > (long)INT_MAX) {
    fprintf(stderr, "Number out of range: ");
    fwrite(text, 1, length, stderr);
    fprintf(stderr, "\n");
    exit(1);
  }
  return (int)n;
}

Term* ParseAtom(ParseInfo* parseInfo, Token* token) {
  Term* term;
  const char* tokenText = parseInfo->code + token->offset;
  switch (token->type) {
    case TOK_IDENTIFIER:
      // TODO: Intern symbols.
      term = NewAtom(T_SYMBOL);
      term->value.string.text = tokenText;
      term->value.string.len = token->length;
      break;
    case TOK_STRING:
      // TODO: Copy strings to a consolidated space.
      term = NewAtom(T_STRING);
      term->value.string.text = tokenText;
      term->value.string.len = token->length;
      break;
    case TOK_NUMBER:
      term = NewAtom(T_NUMBER);
      term->value.number.n = ParseNumber(tokenText, token->length);
      break;
    default:
      fprintf(stderr, "Unexpected token type: %d\n", token->type);
      exit(1);
  }
  return term;
}

Term* ParseList(ParseInfo* parseInfo) {
  Term* listHead = 0;
  Term* listTail = 0;
  for (;;) {
    Token* nextToken = &parseInfo->tokens[parseInfo->nextToken];
    parseInfo->nextToken++;
    if (nextToken->type == TOK_EOF || nextToken->type == TOK_RPAREN) {
      break;
    }
    Term* newNode;
    if (nextToken->type == TOK_LPAREN) {
      newNode = ParseList(parseInfo);
      if (parseInfo->nextToken == parseInfo->tokenCount) {
        fprintf(stderr, "Premature end of file.\n");
        exit(1);
      }
    } else {
      newNode = ParseAtom(parseInfo, nextToken);
    }
    Term* newListPair = NewList(newNode, 0);
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

Term* Parse(const char* code, Token* tokens, int tokenCount) {
  ParseInfo parseInfo;
  parseInfo.code = code;
  parseInfo.tokens = tokens;
  parseInfo.tokenCount = tokenCount;
  parseInfo.nextToken = 0;
  Term* program = ParseList(&parseInfo);
  if (parseInfo.nextToken < parseInfo.tokenCount) {
    fprintf(stderr, "Unmatched right parenthesis.\n");
    exit(1);
  }
  return program;
}

void PrintAtomText(Term* atom) {
  fwrite(atom->value.string.text, 1, atom->value.string.len, stdout);
}

void PrintAtom(Term* atom) {
  assert(IS_ATOM(atom));
  switch (atom->type) {
    case T_SYMBOL:
    case T_STRING:
      PrintAtomText(atom);
      break;
    case T_NUMBER:   printf("%d", atom->value.number.n); break;
    case T_PRIM_FUN: printf("fun"); break;
    case T_PRIM_LET: printf("let"); break;
    case T_PRIM_NIL: printf("nil"); break;
    case T_PRIM_OR:  printf("or"); break;
    case T_LIST: break; /* Won't occur, checked by assert. */
  }
}

void PrintList(Term* list) {
  Term* node = list;
  while (node) {
    if (node != list)
      printf(" ");
    Term* head = HEAD(node);
    if (IS_ATOM(head)) {
      PrintAtom(head);
    } else {
      printf("(");
      PrintList(head);
      printf(")");
    }
    node = TAIL(node);
  }
}

void PrintProgram(Term* program) {
  PrintList(program);
}

