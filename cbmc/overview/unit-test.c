#include <assert.h>

int quartile(int x);

int main() {
  int x;
  __CPROVER_assume(0 <= x && x < 25);
  int result = quartile(x);
  assert(result == 1);
}
