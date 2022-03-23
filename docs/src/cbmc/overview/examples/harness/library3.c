// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#include <assert.h>

void send(int msg);
int receive();

int alpha(int x) {
  int y = receive();
  assert(y > 0);
  return y+1;
}
