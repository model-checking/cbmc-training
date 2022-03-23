#include <limits.h>
main() {
  int x;
  int y;
  if (x + y <= INT_MAX)
    x = x + y;
  if (y <= INT_MAX - x)
    x = x + y;
}
