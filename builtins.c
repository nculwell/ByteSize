
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "datatype.h"

// TODO: Intern symbols.
Term* GetSymbol(const char* name) {
  Term* sym = NewAtom(T_SYMBOL);
  sym->value.string.text = name;
  sym->value.string.len = strlen(name);
  return sym;
}

Term* MakeString(const char* str) {
  Term* s = NewAtom(T_STRING);
  s->value.string.text = str;
  s->value.string.len = strlen(str);
  return s;
}

// Allocate a built-in function object.
static Term* BIFun(const char* name, Term* (*funPtr)(struct Term*)) {
  Term* bif = NewAtom(T_FUN_NATIVE);
  bif->value.bif.funName = name;
  bif->value.bif.funPtr = funPtr;
  return bif;
}

int ListLength(Term* list) {
  if (list && !IS_LIST(list))
    Die("Called ListLength on non-list.");
  int len = 0;
  while (list) {
    len++;
    list = TAIL(list);
  }
  return len;
}

Term* ListHead(Term* args) {
  assert(args);
  Term* list = HEAD(args);
  assert(list);
  assert(IS_LIST(list));
  return HEAD(list);
}

Term* ListTail(Term* args) {
  assert(args);
  Term* list = HEAD(args);
  assert(list);
  assert(IS_LIST(list));
  return TAIL(list);
}

Term* Display(Term* args) {
  while (args) {
    PrintTerm(stdout, HEAD(args));
    args = TAIL(args);
  }
  return 0;
}

Env* BuiltinEnvironment(MemPool* pool) {
  Env* env = 0;
  /* Primitives */
  env = EnvBind(pool, env, GetSymbol("nil"), 0);
  env = EnvBind(pool, env, GetSymbol("fun"), NewAtom(T_PRIM_FUN));
  env = EnvBind(pool, env, GetSymbol("begin"), NewAtom(T_PRIM_BEGIN));
  env = EnvBind(pool, env, GetSymbol("quote"), NewAtom(T_PRIM_QUOTE));
  env = EnvBind(pool, env, GetSymbol("head"), BIFun("head", ListHead));
  env = EnvBind(pool, env, GetSymbol("tail"), BIFun("tail", ListTail));
  /* I/O */
  env = EnvBind(pool, env, GetSymbol("display"), BIFun("display", Display));
  env = EnvBind(pool, env, GetSymbol("newline"), MakeString("\n"));
  return env;
}

/*
TODO:
atom?
eq?
cons
cond
read input
*/

