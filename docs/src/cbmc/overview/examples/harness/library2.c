// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#include <assert.h>

int counter = 0;

int alpha2(int a) {
  counter = counter + 1;
  return a+counter;
}

int beta2(int b) {
  int bb = alpha2(b);
  assert(bb == b + 1);
}
