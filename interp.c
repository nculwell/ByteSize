
/*
Throughout this file, the argument "iTerm"
refers to the term that's currently being
interpreted. More generally, the "i" prefix
identifies something that's being interpreted,
and an "e" suffix (e.g. "eTerm") is a term
that has already been evaluated.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "datatype.h"

static Term* InterpretForm(Term* iTerm, Env* env, MemPool* pool);
static Term* InterpretString(Term* iTerm, Env* env, MemPool* pool);
static Term* InterpretNumber(Term* iTerm, Env* env, MemPool* pool);
static Term* InterpretSymbol(Term* iTerm, Env* env, MemPool* pool);

void Die(const char* message, ...) {
  va_list args;
  va_start(args, message);
  vfprintf(stderr, message, args);
  va_end(args);
  fprintf(stderr, "\n");
  exit(1);
}

void DieShowingTerm(const char* message, Term* term, ...) {
  va_list args;
  va_start(args, term);
  vfprintf(stderr, message, args);
  va_end(args);
  fprintf(stderr, ": ");
  PrintTerm(stderr, term);
  fprintf(stderr, "\n");
  exit(1);
}

void PrintEnv(FILE* f, Env* env, MemPool* pool) {
  while (env) {
    fwrite(env->nameText, 1, env->nameLen, f);
    fprintf(f, " = ");
    PrintTerm(f, env->value);
    fprintf(f, "\n");
    env = env->next;
  }
}

static Term* InterpretTerm(Term* iTerm, Env* env, MemPool* pool) {
  if (!iTerm)
    return 0;
  switch (iTerm->type) {
    case T_PRIM_NIL:
      ; /* Already handled by the null check above. */
    case T_CONS:
      return InterpretForm(iTerm, env);
    case T_STRING:
      return InterpretString(iTerm, env);
    case T_NUMBER:
      return InterpretNumber(iTerm, env);
    case T_SYMBOL:
      return InterpretSymbol(iTerm, env);
    case T_PRIM_FUN:
    case T_PRIM_QUOTE:
    case T_PRIM_BEGIN:
    case T_FUN_NATIVE:
    case T_FUN_USER:
    case T_FUN_MACRO:
      ; /* The parser doesn't generate these. */
  }
  Die("Unexpected term type in InterpretTerm.");
}

static Term* InterpretList(Term* iList, Env* env, MemPool* pool) {
  /* Shortcut for lists with no elements. */
  if (!iList) {
    return 0;
  }
  /* Evaluate list elements in left-to-right order. */
  Term* eListHead = NewCons(InterpretTerm(HEAD(iList), env), 0);
  Term* eListLast = eListHead;
  Term* iListNode = TAIL(iList);
  while (iListNode) {
    eListLast->value.list.tail =
      NewCons(InterpretTerm(HEAD(iListNode), env), 0);
    eListLast = TAIL(eListLast);
    iListNode = TAIL(iListNode);
  }
  return eListHead;
}

static Term* InterpretBifCall(Term* eFun, Term* iArgList, Env* env, MemPool* pool) {
  assert(IS_FUN_NATIVE(eFun));
  Term* eArgList = InterpretList(iArgList, env);
  return eFun->value.bif.funPtr(eArgList);
}

static Term* InterpretUdfCall(Term* eFun, Term* iArgList, Env* env, MemPool* pool) {
  assert(IS_FUN_USER(eFun));
  Term* eArgList = InterpretList(iArgList, env);
  /* Bind function arguments. */
  Env* callEnv = eFun->value.udf.funEnv;
  Term* funArgNames = eFun->value.udf.funArgs;
  while (funArgNames) {
    if (eArgList == NULL) {
      Die("Too few arguments to function.");
    }
    callEnv = EnvBind(pool, env, HEAD(funArgNames), HEAD(eArgList));
    eArgList = TAIL(eArgList);
    funArgNames = TAIL(funArgNames);
  }
  if (eArgList != NULL) {
    Die("Too many arguments to function.");
  }
  /* Invoke the function body. */
  return InterpretTerm(eFun->value.udf.funBody, callEnv);
}

static void ValidateFunArgDecls(Term* funArgDecls) {
  while (funArgDecls) {
    if (!IS_SYMBOL(HEAD(funArgDecls))) {
      Die("Function argument declarations must be symbols.");
    }
    funArgDecls = TAIL(funArgDecls);
  }
}

static Term* InterpretFunctionDef(Term* iFunDef, Env* env, MemPool* pool) {
  if (!iFunDef) {
    Die("Empty function definition.");
  }
  //Term* funName = HEAD(iFunDef); // FIXME: Save name.
  //if (!IS_SYMBOL(funName)) {
  //  Die("Function name must be a symbol.");
  //}
  Term* funArgsAndBody = TAIL(iFunDef);
  if (!funArgsAndBody) {
    Die("Function arguments and body missing.");
  }
  Term* funArgDecls = HEAD(funArgsAndBody);
  ValidateFunArgDecls(funArgDecls);
  Term* funBody = TAIL(funArgsAndBody);
  if (!funBody) {
    Die("Function body missing.");
  }
  Term* eFunDef = (Term*)Alloc(sizeof(Term));
  eFunDef->type = T_FUN_USER;
  //eFunDef->value.udf.funName = funName;
  eFunDef->value.udf.funBody = funBody;
  eFunDef->value.udf.funArgs = funArgDecls;
  eFunDef->value.udf.funEnv = env;
  return eFunDef;
}

static Term* InterpretQuote(Term* iForm, Env* env, MemPool* pool) {
  if (!iForm) {
    Die("Empty quote form.");
  }
  Term* iQuotedTerm = HEAD(iForm);
  Term* iFormTail = TAIL(iForm);
  if (iFormTail) {
    Die("Quote must have only one argument.");
  }
  /* TODO: Support unquote. */
  return iQuotedTerm;
}

static Term* InterpretBegin(Term* iForm, Env* env, MemPool* pool) {
  if (!iForm) {
    return 0;
  }
  for (;;) {
    Term* eFormHead = InterpretTerm(HEAD(iForm), env);
    if (!TAIL(iForm)) {
      return eFormHead;
    }
    iForm = TAIL(iForm);
  }
}

static Term* InterpretForm(Term* iTerm, Env* env, MemPool* pool) {
  /* Interpret the head first, then the head determines
     the interpretation of the rest of the form. */
  Term* eHead = InterpretTerm(HEAD(iTerm), env);
  switch (eHead->type) {
    case T_PRIM_QUOTE:
      return InterpretQuote(TAIL(iTerm), env);
    case T_PRIM_BEGIN:
      return InterpretBegin(TAIL(iTerm), env);
    case T_FUN_NATIVE:
      return InterpretBifCall(eHead, TAIL(iTerm), env);
      break;
    case T_FUN_USER:
      return InterpretUdfCall(eHead, TAIL(iTerm), env);
      break;
    case T_PRIM_FUN:
      return InterpretFunctionDef(TAIL(iTerm), env);
      break;
    default:
      DieShowingTerm("Invalid form", iTerm);
  }
}

static Term* InterpretString(Term* iTerm, Env* env, MemPool* pool) {
  return iTerm;
}

static Term* InterpretNumber(Term* iTerm, Env* env, MemPool* pool) {
  return iTerm;
}

Term* EnvLookup(Env* env, const char* name, int len) {
  Env* envNode = env;
  while (envNode) {
    if (envNode->nameLen == len
        && 0 == strncmp(envNode->nameText, name, len)) {
      return envNode->value;
    }
    envNode = envNode->next;
  }
  return ENV_LOOKUP_FAILED;
}

static Term* InterpretSymbol(Term* iTerm, Env* env, MemPool* pool) {
  assert(IS_SYMBOL(iTerm));
  Term* t = EnvLookup(env, iTerm->value.string.text, iTerm->value.string.len);
  if (t == ENV_LOOKUP_FAILED) {
    DieShowingTerm("Unresolved symbol", iTerm);
  }
  return t;
}

Term* Interpret(Term* iProgram) {
  MemPool* pool = NewMemPool();
  Env* builtinEnv = BuiltinEnvironment(pool);
  printf("--------------------\n");
  printf("Environment:\n");
  PrintEnv(stdout, builtinEnv);
  printf("--------------------\n");
  Term* iWrappedProgram = NewCons(GetSymbol("begin"), iProgram);
  return InterpretTerm(iWrappedProgram, builtinEnv, pool);
}

