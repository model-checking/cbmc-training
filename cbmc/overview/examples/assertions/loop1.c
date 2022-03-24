// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#include<assert.h>

int main() {
  unsigned array[10];

  for (int i = 0; i < 10; i++) {
    array[i] = 0;
  }

  for (int i = 0; i < 10; i++) {
    assert(array[i] == 0);
  }
  return 0;
}
