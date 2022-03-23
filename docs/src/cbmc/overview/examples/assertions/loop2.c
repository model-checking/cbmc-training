// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#include<assert.h>

int main() {
  unsigned bound;
  unsigned array[bound];

  for (int i = 0; i < bound; i++) {
    array[i] = 0;
  }

  for (int i = 0; i < bound; i++) {
    assert(array[i] == 0);
  }
  return 0;
}
