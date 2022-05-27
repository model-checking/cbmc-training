// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#include <stdlib.h>
#define LENGTH 2

void init(int *, size_t);

int main() {
  int vector[LENGTH];

  init(vector, LENGTH);
  for (int i=0; i<=LENGTH; i++)
    vector[i] = vector[i] + 1;

  return 0;
}
