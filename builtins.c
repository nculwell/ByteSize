
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "datatype.h"

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


Env* BuiltinEnvironment() {
  return 0;
}

