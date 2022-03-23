#include <assert.h>

void send(int msg);
int receive();

int alpha(int x) {
  int y = receive();
  assert(y > 0);
  return y+1;
}
