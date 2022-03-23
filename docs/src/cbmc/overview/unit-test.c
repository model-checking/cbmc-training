// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#include <assert.h>

int quartile(int x);

int main() {
  int x;
  __CPROVER_assume(0 <= x && x < 25);
  int result = quartile(x);
  assert(result == 1);
}
