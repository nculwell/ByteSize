
#include <stdio.h>

#define ON_OVERFLOW_GOTO(LABEL) asm goto ("jo %l[" #LABEL "]"::::LABEL)

void mulWithOverflow(int a, int b) {
  int c = a * b;
  ON_OVERFLOW_GOTO(overflow);
  printf("%d * %d = %d\n", a, b, c);
  return;
overflow:
  __attribute__((cold, unused));
  printf("* overflow: %d\n", c);
}

void addWithOverflow(int a, int b) {
  int c = a + b;
  asm goto (
      "jc %l[carry]\r\n"
      "jo %l[overflow]"
      :::: overflow, carry
      );
  printf("%d + %d = %d\n", a, b, c);
  return;
overflow:
  __attribute__((cold, unused));
  printf("+ overflow: %d\n", c);
  return;
carry:
  __attribute__((cold, unused));
  printf("+ carry: %d\n", c);
  return;
}

int main(int argc, char** argv) {
  for (int i=0; i < 32; i++) {
    int x = 1 << i;
    mulWithOverflow(x, x);
    addWithOverflow(x, x);
  }
}

