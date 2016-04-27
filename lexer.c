
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>

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

static const char* keywords[] = {
  "",
  "fun",
  "let",
  "nil",
  "or",
  NULL
};

int MatchKeyword(const char* code, Token* token, int offset) {
  // TODO: Binary search.
  int keywordId = 1;
  int tokenLength = offset - token->offset;
  for (;;) {
    const char* kwd = keywords[keywordId];
    if (kwd == NULL)
      return -1;
    //printf("Keyword search: %s (%Id) vs len %d\n", kwd, strlen(kwd), tokenLength);
    //printf("Strncmp: %d\n", strncmp(kwd, code + token->offset, tokenLength));
    if (strlen(kwd) == tokenLength
        && 0 == strncmp(kwd, code + token->offset, tokenLength))
      return keywordId;
    keywordId++;
  }
}

int SkipWhitespace(const char* code, int offset) {
  for (;;) {
    int c = code[offset];
    if (c != ' ' && c != '\n' && c != '\t')
      break;
    offset++;
  }
  return offset;
}

void NextToken(const char* code, int initialOffset, Token* token) {
  int offset = SkipWhitespace(code, initialOffset);
  token->offset = offset;
  token->type = TOK_ERROR; // initialize type to error as a failsafe
  if (code[offset] == 0) {
    // TOKEN TYPE: End of file.
    token->type = TOK_EOF;
  } else if (isalnum(code[offset])) {
    // TOKEN TYPE: Identifier or number.
    int allDigits = 1;
    for (;;) {
      //printf("Alnum: [%c]\n", code[offset]);
      if (isalpha(code[offset]))
        allDigits = 0;
      else if (isdigit(code[offset]))
        ;
      else
        break;
      offset++;
    }
    if (allDigits) {
      token->type = TOK_NUMBER;
    } else {
      int keywordId = MatchKeyword(code, token, offset);
      if (keywordId == -1)
        token->type = TOK_IDENTIFIER;
      else
        token->type = TOK_KEYWORD + keywordId;
    }
  } else if (code[offset] == '"') {
    // TOKEN TYPE: String.
    offset++;
    for (;;) {
      if (code[offset] == 0) {
        token->type = TOK_ERROR;
        break;
      } else if (code[offset] == '\\') {
        offset++; // skip escaped char
      } else if (code[offset] == '"') {
        offset++; // include terminating quote in token
        break;
      }
      offset++;
    }
    token->type = TOK_STRING;
  } else if (code[offset] == '(') {
    offset++;
    token->type = TOK_LPAREN;
  } else if (code[offset] == ')') {
    offset++;
    token->type = TOK_RPAREN;
  } else {
    // TOKEN TYPE: Error (no match).
    token->type = TOK_ERROR;
  }
  token->length = offset - token->offset;
}

void PrintToken(const char* code, Token* token) {
  switch (token->type) {
    case TOK_KEYWORD_FUN:
      printf("FUN: ");
      break;
    case TOK_KEYWORD_LET:
      printf("LET: ");
      break;
    case TOK_KEYWORD_NIL:
      printf("NIL: ");
      break;
    case TOK_KEYWORD_OR:
      printf("OR: ");
      break;
    case TOK_IDENTIFIER:
      printf("Identifier: ");
      break;
    case TOK_NUMBER:
      printf("Number: ");
      break;
    case TOK_STRING:
      printf("String: ");
      break;
    case TOK_LPAREN:
      printf("L paren: ");
      break;
    case TOK_RPAREN:
      printf("R paren: ");
      break;
    case TOK_ERROR:
      printf("LEX ERROR\n");
      break;
    case TOK_EOF:
      printf("EOF\n");
      break;
    case TOK_KEYWORD:
    case TOK_COUNT:
      ; // these are dummy types
  }
  fwrite(code + token->offset, 1, token->length, stdout);
  printf("\n");
}

void Lex(const char* code) {
  int offset = 0;
  Token token;
  for (;;) {
    NextToken(code, offset, &token);
    offset = token.offset + token.length;
    PrintToken(code, &token);
    if (token.type == TOK_EOF || token.type == TOK_ERROR) {
      break;
    }
  }
}

const char* LoadFile(const char* filename) {
  FILE* f = fopen(filename, "rb");
  if (!f) {
    fprintf(stderr, "Unable to open file: %s\n", filename);
    exit(1);
  }
  fseek(f, 0, SEEK_END);
  size_t fileLength = ftell(f); // TODO: check cast
  rewind(f);
  char* fileContents = (char*)malloc(fileLength + 1);
  if (!fileContents) {
    fprintf(stderr, "Out of memory opening file: %s\n", filename);
    exit(1);
  }
  size_t nRead = 0;
  while (nRead += fread(fileContents + nRead, 1, fileLength - nRead, f), nRead < fileLength) {
  }
  fileContents[fileLength] = 0;
  fclose(f);
  return fileContents;
}

