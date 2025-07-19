#include </home/dry/programming/c/kudo/runtime/kudo.h>
extern int printf(const char *msg, ...);
void foo(int a) { printf("In Foo: %d\n", a); }

int main() {
  int agent = (20) + (2);
  printf("agent: %d\n", agent);
  foo(40);
}
