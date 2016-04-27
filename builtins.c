
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "datatype.h"

/* Generate a built-in symbol. */
static Term* BISymbol(const char* name) {
  Term* sym = (Term*)Alloc(sizeof(Term));
  sym->type = T_SYMBOL;
  sym->value.string.text = name;
  sym->value.string.len = strlen(name);
  return sym;
}

static Term* BIFun(const char* name, Term* (*funPtr)(struct Term*)) {
  Term* bif = (Term*)Alloc(sizeof(Term));
  bif->type = T_FUN_NATIVE;
  bif->value.bif.funName = name;
  bif->value.bif.funPtr = funPtr;
  return bif;
}

int ListLength(Term* list) {
  if (list && !IS_LIST(list))
    die("Called ListLength on non-list.");
  int len = 0;
  while (list) {
    len++;
    list = TAIL(list);
  }
  return len;
}

Term* ListHead(Term* list) {
  assert(list);
  assert(IS_LIST(list));
  return HEAD(list);
}

Term* ListTail(Term* list) {
  assert(list);
  assert(IS_LIST(list));
  return TAIL(list);
}

Env* BuiltinEnvironment() {
  Env* env = 0;
  env = EnvBind(env, BISymbol("nil"), 0);
  env = EnvBind(env, BISymbol("head"), BIFun("head", ListHead));
  return env;
}

