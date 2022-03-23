#include <stdbool.h>
main() {
  int bound;
  for (int i=0; i<bound; i++)
    if (i > 9) assert(false);
}
