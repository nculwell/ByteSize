
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "datatype.h"

typedef struct MacroExpansion {
  Term* term;
  Env* env;
} MacroExpansion;

MacroExpansion MacroExpandList(Term* iList, Env* env);
MacroExpansion MacroExpandForm(Term* iTerm, Env* env);
MacroExpansion MacroExpandTerm(Term* iTerm, Env* env);

Term* MacroExpand(Term* iProgram) {
  // TODO: Builtin environment.
  MacroExpansion ex = MacroExpandTerm(iProgram, 0);
  return ex.term;
}

static MacroExpansion MacroExpandTerm(Term* iTerm, Env* env) {
  MacroExpansion ex;
  ex.term = 0;
  ex.env = env;
  if (!iTerm)
    return ex;
  switch (iTerm->type) {
    case T_PRIM_NIL:
      ; /* Already handled by the null check above. */
    case T_CONS:
      return MacroExpandForm(iTerm, env);
    case T_STRING:
    case T_NUMBER:
    case T_SYMBOL:
      ex.term = iTerm;
      return ex;
    case T_PRIM_FUN:
    case T_PRIM_QUOTE:
    case T_PRIM_BEGIN:
    case T_FUN_NATIVE:
    case T_FUN_USER:
      ; /* The parser doesn't generate these. */
  }
  Die("Unexpected term type in MacroExpandTerm.");
}

static MacroExpansion MacroExpandForm(Term* iTerm, Env* env) {
  assert(IS_CONS(iTerm));
  Term* eFormArgs = MacroExpandList(TAIL(iTerm), env);
  if (HEAD(iTerm)->type == T_SYMBOL) {
    Term* eSymValue =
      EnvLookup(env, iTerm->value.string.text, iTerm->value.string.len);
    if (IS_MACRO(eSymValue)) {
      // We have a macro to expand.
    }
  } else {
  }
  switch (HEAD(iTerm)->type) {
    case T_SYMBOL:
      // See if this symbol refers to a macro.
      {
      }
      break;
    case T_PRIM_QUOTE:
    case T_PRIM_BEGIN:
    case T_FUN_NATIVE:
    case T_FUN_USER:
      return MacroExpandUdfCall(eHead, TAIL(iTerm), env);
      break;
    case T_PRIM_FUN:
      return MacroExpandFunctionDef(TAIL(iTerm), env);
      break;
    default:
      DieShowingTerm("Invalid form", iTerm);
  }
}

static MacroExpansion MacroExpandList(Term* iList, Env* env) {
  MacroExpansion ex;
  ex.term = 0;
  ex.env = env;
  /* Shortcut for lists with no elements. */
  if (!iList) {
    return ex;
  }
  /* Evaluate list elements in left-to-right order. */
  Term* eListHead = NewCons(MacroExpandTerm(HEAD(iList), env), 0);
  Term* eListLast = eListHead;
  Term* iListNode = TAIL(iList);
  while (iListNode) {
    eListLast->value.list.tail =
      NewCons(MacroExpandTerm(HEAD(iListNode), env), 0);
    eListLast = TAIL(eListLast);
    iListNode = TAIL(iListNode);
  }
  ex.term = eListHead;
  return ex;
}

