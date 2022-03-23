#include<assert.h>

int main() {
  unsigned bound;
  unsigned array[bound];

  __CPROVER_assume(bound < 11);

  for (int i = 0; i < bound; i++) {
    array[i] = 0;
  }

  for (int i = 0; i < bound; i++) {
    assert(array[i] == 0);
  }
  return 0;
}
