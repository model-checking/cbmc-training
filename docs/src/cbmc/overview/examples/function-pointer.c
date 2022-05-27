// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#include <assert.h>

int alpha(int x) {
  return x+1;
}

int beta(int x) {
  return x+2;
}

main() {
  int (*function)();

  int bool;
  function = bool ? alpha : beta;

  int x;
  int rc = function(x);
}
