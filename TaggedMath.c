
#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

#define ON_OVERFLOW_GOTO(LABEL) \
  asm goto ("jo %l[" #LABEL "]"::::LABEL)

typdef struct BigNum {
  uint16_t sign;
  uint16_t len;
  uint32_t exp;
  uint64_t n[1];
} BigNum;

BigNum* BigNumPromote(Number a) {
  BigNum* b = (BigNum*)malloc(sizeof(BigNum));
  b->len = 1;
  b->exp = 0;
  b->n[1] = a.i;
  return b;
}

Number BigNumAdd(BigNum* a, BigNum* b) {
  uint16_t len = a->len > b->len ? a->len : b->len;
  BigNum* c = (BigNum*)malloc((len + 1) * sizeof(BigNum));
  c->len = a->len > b->len ? a->len+1 : b->len+1;
}

typedef union Number {
  int32_t i;
  BigNum* b;
} Number;

Number add(Number a, Number b) {
  Number c;
  if (a.i & 1) {
    if (b.i & 1) {
      // Both are tagged ints.
      c.i = a.i + b.i;
      ON_OVERFLOW_GOTO(overflow);
      c.i >> 1;
      return c;
overflow:
      return BigNumAdd(BigNumPromote(a), BigNumPromote(b));
    } else {
      return BigNumAdd(BigNumPromote(a), b.b);
    }
  } else {
    if (b.i & 1) {
      return BigNumAdd(a.b, BigNumPromote(b));
    } else {
      return BigNumAdd(a.b, b.b);
    }
  }
}

