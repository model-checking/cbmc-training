// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#include <assert.h>

int alpha1(int a) {
  return a+1;
}

int beta1(int b) {
  int bb = alpha1(b);
  assert(bb == b);
}
